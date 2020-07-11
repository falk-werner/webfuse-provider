#ifndef WFP_JSON_UTIL_H
#define WFP_JSON_UTIL_H

#include <jansson.h>
#include "webfuse_provider/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern int
wfp_impl_json_get_integer(
    json_t const * object,
    char const * key,
    int default_value);

extern wfp_status 
wfp_impl_jsonrpc_get_status(
    json_t const * error);

#ifdef __cplusplus
}
#endif

#endif
