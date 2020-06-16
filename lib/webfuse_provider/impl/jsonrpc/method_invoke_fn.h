#ifndef WFP_JSONRPC_METHOD_INVOKE_FN_H
#define WFP_JSONRPC_METHOD_INVOKE_FN_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_jsonrpc_request;

typedef void wfp_jsonrpc_method_invoke_fn(
    struct wfp_jsonrpc_request * request,
    char const * method_name,
    json_t * params,
    void * user_data);

#ifdef __cplusplus
}
#endif


#endif
