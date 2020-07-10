#ifndef WFP_JSONRPC_SEND_FN_H
#define WFP_JSONRPC_SEND_FN_H

#ifndef __cplusplus
#include <stdbool.h>
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef void wfp_jsonrpc_send_fn(
	char * message,
    size_t length,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
