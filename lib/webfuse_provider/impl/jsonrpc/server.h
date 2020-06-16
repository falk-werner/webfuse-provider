#ifndef WFP_JSONRPC_SERVER_H
#define WFP_JSONRPC_SERVER_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stdbool.h>
#else
#include <cstdarg>
#endif

#include <jansson.h>
#include "webfuse_provider/impl/jsonrpc/method_invoke_fn.h"
#include "webfuse_provider/impl/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_jsonrpc_server;

extern struct wfp_jsonrpc_server *
wfp_jsonrpc_server_create(void);

extern void
wfp_jsonrpc_server_dispose(
    struct wfp_jsonrpc_server * server);

extern void wfp_jsonrpc_server_add(
    struct wfp_jsonrpc_server * server,
    char const * method_name,
    wfp_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void wfp_jsonrpc_server_process(
    struct wfp_jsonrpc_server * server,
    json_t * request,
    wfp_jsonrpc_send_fn * send,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
