#include "webfuse_provider/impl/jsonrpc/method.h"
#include <stdlib.h>
#include <string.h>

struct wfp_jsonrpc_method * wfp_jsonrpc_method_create(
    char const * method_name,
    wfp_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wfp_jsonrpc_method * method = malloc(sizeof(struct wfp_jsonrpc_method));
    method->next = NULL;
    method->name = strdup(method_name);
    method->invoke = invoke;
    method->user_data = user_data;

    return method;
}

void wfp_jsonrpc_method_dispose(
    struct wfp_jsonrpc_method * method)
{
    free(method->name);
    free(method);
}
