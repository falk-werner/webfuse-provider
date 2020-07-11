#ifndef WFP_JSONRPC_REQUEST_H
#define WFP_JSONRPC_REQUEST_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json;

extern bool wfp_jsonrpc_is_request(
    struct wfp_json const * message);

#ifdef __cplusplus
}
#endif

#endif
