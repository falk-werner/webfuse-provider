#include "webfuse_provider/impl/jsonrpc/proxy_intern.h"
#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/impl/jsonrpc/error.h"
#include "webfuse_provider/status.h"

#include "webfuse_provider/impl/timer/timer.h"
#include "webfuse_provider/impl/json/writer.h"

#include <libwebsockets.h>

#include <stdlib.h>
#include <string.h>

struct wfp_jsonrpc_proxy *
wfp_jsonrpc_proxy_create(
    struct wfp_timer_manager * manager,
    int timeout,
    wfp_jsonrpc_send_fn * send,
    void * user_data)
{
    struct wfp_jsonrpc_proxy * proxy = malloc(sizeof(struct wfp_jsonrpc_proxy));
    wfp_jsonrpc_proxy_init(proxy, manager, timeout, send, user_data);

    return proxy;
}

void wfp_jsonrpc_proxy_dispose(
    struct wfp_jsonrpc_proxy * proxy)
{
    wfp_jsonrpc_proxy_cleanup(proxy);
    free(proxy);
}

static void wfp_jsonrpc_proxy_on_timeout(
    struct wfp_timer * timer, void * proxy_ptr)
{
    struct wfp_jsonrpc_proxy * proxy = proxy_ptr;

    if (proxy->request.is_pending)
    {
        wfp_jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wfp_timer_cancel(timer);

        wfp_jsonrpc_propate_error(finished, user_data, WFP_BAD_TIMEOUT, "Timeout");
    }
}

static char * wfp_jsonrpc_request_create(
    size_t * length,
	char const * method,
	int id,
	char const * param_info,
	va_list args)
{
    struct wfp_json_writer * writer = wfp_impl_json_writer_create(128, LWS_PRE);
    wfp_impl_json_writer_object_begin(writer);

    wfp_impl_json_writer_object_key(writer, "method");
    wfp_impl_json_writer_write_string(writer, method);
	
    wfp_impl_json_writer_object_key(writer, "params");
    wfp_impl_json_writer_array_begin(writer);
	for (char const * param_type = param_info; '\0' != *param_type; param_type++)
	{
		switch(*param_type)
		{
			case 's':
			{
				char const * const value = va_arg(args, char const *);
                wfp_impl_json_writer_write_string(writer, value);
			}
			break;
			case 'i':
			{
				int const value = va_arg(args, int);
                wfp_impl_json_writer_write_int(writer, value);
			}
			break;
            case 'j':
            {
                wfp_jsonrpc_custom_write_fn * write = va_arg(args, wfp_jsonrpc_custom_write_fn *);
                void * data = va_arg(args, void *);
                write(writer,data);
            }
            break;
			default:
			fprintf(stderr, "fatal: unknown param_type '%c'\n", *param_type);
            break;
		}
	}
    wfp_impl_json_writer_array_end(writer);

    if (0 != id)
    {
        wfp_impl_json_writer_object_key(writer, "id");
        wfp_impl_json_writer_write_int(writer, id);
    }

    wfp_impl_json_writer_object_end(writer);

    char * message = wfp_impl_json_writer_take_data(writer, length);
    wfp_impl_json_writer_dispose(writer);

    return message;
}

void wfp_jsonrpc_proxy_init(
    struct wfp_jsonrpc_proxy * proxy,
    struct wfp_timer_manager * timeout_manager,
    int timeout,
    wfp_jsonrpc_send_fn * send,
    void * user_data)
{
    proxy->send = send;
    proxy->timeout = timeout;
    proxy->user_data = user_data;
    proxy->request.is_pending = false;
    proxy->request.timer = wfp_timer_create(timeout_manager, 
        &wfp_jsonrpc_proxy_on_timeout, proxy);    
}

void wfp_jsonrpc_proxy_cleanup(
    struct wfp_jsonrpc_proxy * proxy)
{
    if (proxy->request.is_pending)
    {
        void * user_data = proxy->request.user_data;
        wfp_jsonrpc_proxy_finished_fn * finished = proxy->request.finished;

        proxy->request.is_pending = false;
        proxy->request.finished = NULL;
        proxy->request.user_data = NULL;
        proxy->request.id = 0;
        wfp_timer_cancel(proxy->request.timer);

        wfp_jsonrpc_propate_error(finished, user_data, WFP_BAD, "Bad: cancelled pending request during shutdown");
    }

    wfp_timer_dispose(proxy->request.timer);
}

void wfp_jsonrpc_proxy_vinvoke(
	struct wfp_jsonrpc_proxy * proxy,
	wfp_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args)
{
    if (!proxy->request.is_pending)
    {
        proxy->request.is_pending = true;
        proxy->request.finished = finished;
        proxy->request.user_data = user_data;
        proxy->request.id = 42;
        wfp_timer_start(proxy->request.timer, proxy->timeout);
        
        size_t length;
        char * message = wfp_jsonrpc_request_create(&length, method_name, proxy->request.id, param_info, args);
        
        proxy->send(message, length, proxy->user_data);
    }
    else
    {
        wfp_jsonrpc_propate_error(finished, user_data, WFP_BAD_BUSY, "Busy");
    }
}

extern void wfp_jsonrpc_proxy_vnotify(
	struct wfp_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args)
{
    size_t length;
    char * request = wfp_jsonrpc_request_create(&length, method_name, 0, param_info, args);

    proxy->send(request, length, proxy->user_data);
}


void wfp_jsonrpc_proxy_onresult(
    struct wfp_jsonrpc_proxy * proxy,
    json_t * message)
{
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, message);

    if ((proxy->request.is_pending) && (response.id == proxy->request.id))
    {
        wfp_jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wfp_timer_cancel(proxy->request.timer);

        finished(user_data, response.result, response.error);
    }

    wfp_jsonrpc_response_cleanup(&response);
}

