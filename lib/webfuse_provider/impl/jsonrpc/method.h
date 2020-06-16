#ifndef WFP_JSONRPC_METHOD_H
#define WFP_JSONRPC_METHOD_H

#include "webfuse_provider/impl/jsonrpc/method_invoke_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_jsonrpc_method
{
    struct wfp_jsonrpc_method * next;
    char * name;
    wfp_jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

extern struct wfp_jsonrpc_method *
wfp_jsonrpc_method_create(
    char const * method_name,
    wfp_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void
wfp_jsonrpc_method_dispose(
    struct wfp_jsonrpc_method * method);

#ifdef __cplusplus
}
#endif


#endif
