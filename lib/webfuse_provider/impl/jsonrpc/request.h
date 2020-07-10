#ifndef WFP_JSONRPC_REQUEST_H
#define WFP_JSONRPC_REQUEST_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstdarg>
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>
#include "webfuse_provider/impl/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_jsonrpc_request;

extern bool wfp_jsonrpc_is_request(
    json_t * message);

extern struct wfp_jsonrpc_request *
wfp_jsonrpc_request_create(
    int id,
    wfp_jsonrpc_send_fn * send,
    void * user_data);

extern void wfp_jsonrpc_request_dispose(
    struct wfp_jsonrpc_request * request);

extern void * wfp_jsonrpc_request_get_userdata(
    struct wfp_jsonrpc_request * request);

#ifdef __cplusplus
}
#endif

#endif
