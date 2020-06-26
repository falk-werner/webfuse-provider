#ifndef WFP_JSONRPC_ERROR_H
#define WFP_JSONRPC_ERROR_H

#include <jansson.h>
#include "webfuse_provider/impl/jsonrpc/proxy_finished_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern json_t *
wfp_jsonrpc_error(
    int code,
    char const * message);

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
