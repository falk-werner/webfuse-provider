#ifndef WFP_JSONRPC_ERROR_H
#define WFP_JSONRPC_ERROR_H

#include "webfuse_provider/impl/jsonrpc/proxy_finished_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_jsonrpc_error
{
    int code;
    char * message;
};

extern struct wfp_jsonrpc_error *
wfp_jsonrpc_error_create(
    int code,
    char const * message);

extern void
wfp_jsonrpc_error_dispose(
    struct wfp_jsonrpc_error * error);

extern void
wfp_jsonrpc_propate_error(
    wfp_jsonrpc_proxy_finished_fn * finised,
    void * user_data,
    int code,
    char const * message);

#ifdef __cplusplus
}
#endif


#endif
