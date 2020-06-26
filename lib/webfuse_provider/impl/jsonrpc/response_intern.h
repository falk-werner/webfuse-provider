#ifndef WFP_JSONRPC_RESPONSE_INTERN_H
#define WFP_JSONRPC_RESPONSE_INTERN_H

#include "webfuse_provider/impl/jsonrpc/response.h"

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct wfp_jsonrpc_response
{
	json_t * result;	
	json_t * error;
	int id;
};

extern void wfp_jsonrpc_response_init(
	struct wfp_jsonrpc_response * response,
    json_t * message);

extern void wfp_jsonrpc_response_cleanup(
	struct wfp_jsonrpc_response * response);

#ifdef __cplusplus
}
#endif

#endif

