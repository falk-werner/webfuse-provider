#ifndef WFP_JSONRPC_RESPONSE_H
#define WFP_JSONRPC_RESPONSE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json;

extern bool
wfp_jsonrpc_is_response(
    struct wfp_json const * message);

#ifdef __cplusplus
}
#endif

#endif
