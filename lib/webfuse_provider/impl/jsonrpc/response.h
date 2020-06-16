#ifndef WFP_JSONRPC_RESPONSE_H
#define WFP_JSONRPC_RESPONSE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern bool wfp_jsonrpc_is_response(
    json_t * message);

#ifdef __cplusplus
}
#endif

#endif
