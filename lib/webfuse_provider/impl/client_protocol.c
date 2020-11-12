#include "webfuse_provider/impl/client_protocol.h"

#include <stdlib.h>
#include <string.h>

#include <libwebsockets.h>

#include "webfuse_provider/impl/client_config.h"
#include "webfuse_provider/impl/provider.h"
#include "webfuse_provider/impl/credentials.h"
#include "webfuse_provider/impl/util/util.h"
#include "webfuse_provider/impl/message.h"
#include "webfuse_provider/impl/message_queue.h"
#include "webfuse_provider/impl/util/container_of.h"
#include "webfuse_provider/impl/util/url.h"
#include "webfuse_provider/protocol_names.h"

#include "webfuse_provider/impl/timer/manager.h"

#include "webfuse_provider/impl/jsonrpc/response.h"
#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/impl/jsonrpc/proxy.h"
#include "webfuse_provider/impl/json/doc.h"

#define WFP_DEFAULT_TIMEOUT (10 * 1000)

static void wfp_impl_client_protocol_respond(
    struct wfp_message * message,
    void * user_data)
{
    struct wfp_client_protocol * protocol = (struct wfp_client_protocol *) user_data;

    wfp_slist_append(&protocol->messages, &message->item);
    lws_callback_on_writable(protocol->wsi);
}

static void wfp_impl_client_protocol_process(
     struct wfp_client_protocol * protocol,
     char * data,
     size_t length)
{
    struct wfp_json_doc * doc = wfp_impl_json_doc_loadb(data, length);
    if (NULL != doc)
    {
        struct wfp_json const * message = wfp_impl_json_doc_root(doc);
        if (wfp_jsonrpc_is_response(message))
        {
            wfp_jsonrpc_proxy_onresult(protocol->proxy, message);
        }

        if (wfp_jsonrpc_is_request(message))
        {
            struct wfp_impl_invokation_context context =
            {
                .provider = &protocol->provider,
                .user_data = protocol->user_data,
                .request = &protocol->request
            };

            wfp_impl_provider_invoke(&context, message);
        }

        wfp_impl_json_doc_dispose(doc);
    }
}

static void
wfp_impl_client_protocol_on_add_filesystem_finished(
	void * user_data,
	struct wfp_json const * result,
	struct wfp_jsonrpc_error const * WFP_UNUSED_PARAM(error))
{
    struct wfp_client_protocol * protocol = user_data;
    if (NULL == protocol->wsi) { return; }

    if (NULL != result)
    {
        protocol->provider.log(protocol->user_data, 0, "filesystem added");
        protocol->provider.log(protocol->user_data, 0, "handshake finished");
        protocol->is_connected = true;
        protocol->provider.connected(protocol->user_data);
    }
    else
    {
        protocol->provider.log(protocol->user_data, 0, "add filesystem failed");
        protocol->is_shutdown_requested = true;
        lws_callback_on_writable(protocol->wsi);
    }
}

static void wfp_impl_client_protocol_add_filesystem(
     struct wfp_client_protocol * protocol)
{
    protocol->provider.log(protocol->user_data, 0, "begin add filesystem");

    wfp_jsonrpc_proxy_invoke(
        protocol->proxy,
        &wfp_impl_client_protocol_on_add_filesystem_finished,
        protocol,
        "add_filesystem",
        "s",
        protocol->fs_name);
}

static void
wfp_impl_client_protocol_on_authenticate_finished(
	void * user_data,
	struct wfp_json const * result,
	struct wfp_jsonrpc_error const * WFP_UNUSED_PARAM(error))
{
    struct wfp_client_protocol * protocol = user_data;
    if (NULL == protocol->wsi) { return; }

    if (NULL != result)
    {
        protocol->provider.log(protocol->user_data, 0, "authentication finished");
        wfp_impl_client_protocol_add_filesystem(protocol);
    }
    else
    {
        protocol->provider.log(protocol->user_data, 0, "authentication failed");
        protocol->is_shutdown_requested = true;
        lws_callback_on_writable(protocol->wsi);
    }
}

static void wfp_impl_client_protocol_authenticate(
    struct wfp_client_protocol * protocol)
{
    protocol->provider.log(protocol->user_data, 0, "begin authenticate");

    struct wfp_credentials credentials;
    wfp_impl_credentials_init(&credentials);

    protocol->provider.get_credentials(&credentials, protocol->user_data);

    char const * cred_type = wfp_impl_credentials_get_type(&credentials);

    wfp_jsonrpc_proxy_invoke(
        protocol->proxy,
        &wfp_impl_client_protocol_on_authenticate_finished,
        protocol,
        "authenticate",
        "sj",
        cred_type, &wfp_impl_credentials_write, &credentials);

    wfp_impl_credentials_cleanup(&credentials);
}

static void wfp_impl_client_protocol_handshake(
    struct wfp_client_protocol * protocol)
{
    protocol->provider.log(protocol->user_data, 0, "begin connection handshake");

    if (wfp_impl_provider_is_authentication_enabled(&protocol->provider))
    {
        wfp_impl_client_protocol_authenticate(protocol);
    }
    else
    {
        protocol->provider.log(protocol->user_data, 0, "skip authentication");
        wfp_impl_client_protocol_add_filesystem(protocol);
    }
}

static int wfp_impl_client_protocol_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WFP_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    int result = 0;
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    struct wfp_client_protocol * protocol = (NULL != ws_protocol) ? ws_protocol->user: NULL;

    if (NULL != protocol)
    {
        wfp_timer_manager_check(protocol->timer_manager);

        switch (reason)
        {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            protocol->provider.log(protocol->user_data, 0, "connection established");
            wfp_impl_client_protocol_handshake(protocol);
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            protocol->provider.log(protocol->user_data, 0, "connect error");
            protocol->is_connected = false;
            protocol->provider.disconnected(protocol->user_data);
            break;
        case LWS_CALLBACK_CLIENT_CLOSED:
            protocol->provider.log(protocol->user_data, 0, "connection closed");
            protocol->is_connected = false;
            protocol->provider.disconnected(protocol->user_data);
            protocol->wsi = NULL;
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            protocol->provider.log(protocol->user_data, 0, "received: \"%.*s\"", len, in);
            wfp_impl_client_protocol_process(protocol, in, len);
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            // fall-through
        case LWS_CALLBACK_CLIENT_WRITEABLE:
			if (wsi == protocol->wsi)
            {
                if (protocol->is_shutdown_requested)
                {
                    result = 1;
                }
                else if (!wfp_slist_empty(&protocol->messages))
                {

                    struct wfp_slist_item * item = wfp_slist_remove_first(&protocol->messages);
                    struct wfp_message * message = wfp_container_of(item, struct wfp_message, item);
                    protocol->provider.log(protocol->user_data, 0, "send: \"%.*s\"", message->length, message->data);
                    lws_write(wsi, (unsigned char*) message->data, message->length, LWS_WRITE_TEXT);
                    wfp_message_dispose(message);

                    if (!wfp_slist_empty(&protocol->messages))
                    {
                        lws_callback_on_writable(wsi);
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    return result;
}

static void wfp_impl_client_protocol_send(
    char * data,
    size_t length,
    void * user_data)
{
    struct wfp_client_protocol * protocol = user_data;

    struct wfp_message * message = wfp_message_create(data, length);
    wfp_slist_append(&protocol->messages, &message->item);
    lws_callback_on_writable(protocol->wsi);
}

void wfp_impl_client_protocol_init(
    struct wfp_client_protocol * protocol,
    struct wfp_provider const * provider,
    char const * fs_name,
    void * user_data)
{
    protocol->is_connected = false;
    protocol->is_shutdown_requested = false;
    wfp_slist_init(&protocol->messages);

    protocol->wsi = NULL;

    protocol->request.respond = &wfp_impl_client_protocol_respond;
    protocol->request.user_data = protocol;

    protocol->timer_manager = wfp_timer_manager_create();
    protocol->proxy = wfp_jsonrpc_proxy_create(protocol->timer_manager, WFP_DEFAULT_TIMEOUT, &wfp_impl_client_protocol_send, protocol);

    protocol->fs_name = strdup(fs_name);
    protocol->user_data = user_data;
    wfp_impl_provider_init_from_prototype(&protocol->provider, provider);
}

void wfp_impl_client_protocol_cleanup(
    struct wfp_client_protocol * protocol)
{
    wfp_jsonrpc_proxy_dispose(protocol->proxy);
    wfp_timer_manager_dispose(protocol->timer_manager);
    wfp_message_queue_cleanup(&protocol->messages);
    free(protocol->fs_name);
}

struct wfp_client_protocol * wfp_impl_client_protocol_create(
    struct wfp_client_config const * config)
{
    struct wfp_client_protocol * protocol = malloc(sizeof(struct wfp_client_protocol));
    wfp_impl_client_protocol_init(protocol, &config->provider, config->fs_name, config->user_data);

    return protocol;
}

void wfp_impl_client_protocol_dispose(
    struct wfp_client_protocol * protocol)
{
    wfp_impl_client_protocol_cleanup(protocol);
    free(protocol);
}

void wfp_impl_client_protocol_init_lws(
    struct wfp_client_protocol * protocol,
    struct lws_protocols * lws_protocol)
{
    lws_protocol->name = WFP_PROTOCOL_NAME_PROVIDER_CLIENT;
	lws_protocol->callback = &wfp_impl_client_protocol_callback;
	lws_protocol->per_session_data_size = 0;
	lws_protocol->user = protocol;
}

void wfp_impl_client_protocol_connect(
    struct wfp_client_protocol * protocol,
    struct lws_context * context,
    char const * url)
{
    struct wfp_url url_data;
    bool const success = wfp_url_init(&url_data, url);
    if (success)
    {
        struct lws_client_connect_info info;
        memset(&info, 0, sizeof(struct lws_client_connect_info));
        info.context = context;
        info.port = url_data.port;
        info.address = url_data.host;
        info.path = url_data.path;
        info.host = info.address;
        info.origin = info.address;
        info.ssl_connection = (url_data.use_tls) ? LCCSCF_USE_SSL : 0;
        info.protocol = WFP_PROTOCOL_NAME_ADAPTER_SERVER;
        info.local_protocol_name = WFP_PROTOCOL_NAME_PROVIDER_CLIENT;
        info.pwsi = &protocol->wsi;

        lws_client_connect_via_info(&info);

        wfp_url_cleanup(&url_data);
    }
    else
    {
        protocol->provider.disconnected(protocol->user_data);
    }

}

void wfp_impl_client_protocol_disconnect(
    struct wfp_client_protocol * protocol)
{
    if (protocol->is_connected)
    {
        protocol->is_shutdown_requested = true;
        lws_callback_on_writable(protocol->wsi);
    }
    else
    {
        protocol->provider.disconnected(protocol->user_data);
    }
}
