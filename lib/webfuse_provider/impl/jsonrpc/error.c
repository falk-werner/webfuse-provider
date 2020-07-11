#include "webfuse_provider/impl/jsonrpc/error.h"

#include <stdlib.h>
#include <string.h>

struct wfp_jsonrpc_error *
wfp_jsonrpc_error_create(
    int code,
    char const * message)
{
    struct wfp_jsonrpc_error * error = malloc(sizeof(struct wfp_jsonrpc_error));
    error->code = code;
    error->message = strdup(message);

    return error;
}

void
wfp_jsonrpc_error_dispose(
    struct wfp_jsonrpc_error * error)
{
    free(error->message);
    free(error);
}

void
wfp_jsonrpc_propate_error(
    wfp_jsonrpc_proxy_finished_fn * finised,
    void * user_data,
    int code,
    char const * message)
{
    struct wfp_jsonrpc_error error;
    error.code = code;
    error.message = (char*) message;

    finised(user_data, NULL, &error);
}

