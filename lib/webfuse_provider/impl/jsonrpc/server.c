#include "webfuse_provider/impl/jsonrpc/server.h"
#include "webfuse_provider/impl/jsonrpc/method.h"
#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/status.h"
#include "webfuse_provider/impl/util.h"

#include <stdlib.h>
#include <string.h>

struct wfp_jsonrpc_server
{
    struct wfp_jsonrpc_method * methods;
};

static void
wfp_jsonrpc_server_init(
    struct wfp_jsonrpc_server * server);

static void
wfp_jsonrpc_server_cleanup(
    struct wfp_jsonrpc_server * server);

struct wfp_jsonrpc_server *
wfp_jsonrpc_server_create(void)
{
    struct wfp_jsonrpc_server * server = malloc(sizeof(struct wfp_jsonrpc_server));
    wfp_jsonrpc_server_init(server);

    return server;
}

void
wfp_jsonrpc_server_dispose(
    struct wfp_jsonrpc_server * server)
{
    wfp_jsonrpc_server_cleanup(server);
    free(server);
}

static void wfp_jsonrpc_server_init(
    struct wfp_jsonrpc_server * server)
{
    server->methods = NULL;
}

static void wfp_jsonrpc_server_cleanup(
    struct wfp_jsonrpc_server * server)
{
    struct wfp_jsonrpc_method * current = server->methods;
    while (NULL != current)
    {
        struct wfp_jsonrpc_method * next = current->next;
        wfp_jsonrpc_method_dispose(current);
        current = next;
    }
    server->methods = NULL;
}

void wfp_jsonrpc_server_add(
    struct wfp_jsonrpc_server * server,
    char const * method_name,
    wfp_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wfp_jsonrpc_method * method = wfp_jsonrpc_method_create(method_name, invoke, user_data);
    method->next = server->methods;
    server->methods = method;
}

static void wfp_jsonrpc_server_invalid_method_invoke(
    struct wfp_jsonrpc_request * request,
    char const * WFP_UNUSED_PARAM(method_name),
    json_t * WFP_UNUSED_PARAM(params),
    void * WFP_UNUSED_PARAM(user_data))
{
    wfp_jsonrpc_respond_error(request, WFP_BAD_NOTIMPLEMENTED, "not implemented");
}

static struct wfp_jsonrpc_method const wfp_jsonrpc_server_invalid_method = 
{
    .next = NULL,
    .name = "<invalid>",
    .invoke = &wfp_jsonrpc_server_invalid_method_invoke,
    .user_data = NULL    
};

static struct wfp_jsonrpc_method const *
wfp_jsonrpc_server_get_method(
    struct wfp_jsonrpc_server * server,
    char const * method_name)
{
    struct wfp_jsonrpc_method const * current = server->methods;
    while (NULL != current) 
    {
        if (0 == strcmp(method_name, current->name))
        {
            return current;
        }

        current = current->next;
    }

    return &wfp_jsonrpc_server_invalid_method;
}

void wfp_jsonrpc_server_process(
    struct wfp_jsonrpc_server * server,
    json_t * request_data,
    wfp_jsonrpc_send_fn * send,
    void * user_data)
{
    json_t * method_holder = json_object_get(request_data, "method");
    json_t * params = json_object_get(request_data, "params");
    json_t * id_holder = json_object_get(request_data, "id");

    if (json_is_string(method_holder) &&
        (json_is_array(params) || (json_is_object(params))) &&
        json_is_integer(id_holder))
    {
        char const * method_name = json_string_value(method_holder);
        int id = json_integer_value(id_holder);
        struct wfp_jsonrpc_request * request = wfp_jsonrpc_request_create(id, send, user_data);
        struct wfp_jsonrpc_method const * method = wfp_jsonrpc_server_get_method(server, method_name);

        method->invoke(request, method_name, params, method->user_data);
    }
}

