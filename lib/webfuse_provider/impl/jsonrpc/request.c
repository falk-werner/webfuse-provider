#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/impl/jsonrpc/error.h"
#include <stdlib.h>

struct wfp_jsonrpc_request
{
    int id;
    wfp_jsonrpc_send_fn * send;
    void * user_data;
};

bool
wfp_jsonrpc_is_request(
    json_t * message)
{
    json_t * id = json_object_get(message, "id");
    json_t * method = json_object_get(message, "method");
    json_t * params = json_object_get(message, "params");

    return (json_is_integer(id) && json_is_string(method) &&
            (json_is_array(params) || json_is_object(params)));
}


void
wfp_jsonrpc_request_dispose(
    struct wfp_jsonrpc_request * request)
{
    free(request);
}

void *
wfp_jsonrpc_request_get_userdata(
    struct wfp_jsonrpc_request * request)
{
    return request->user_data;
}
