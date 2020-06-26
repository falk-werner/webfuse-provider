#ifndef WFP_JSONRPC_PROXY_INTERN_H
#define WFP_JSONRPC_PROXY_INTERN_H

#include "webfuse_provider/impl/jsonrpc/proxy.h"
#include "webfuse_provider/impl/jsonrpc/proxy_finished_fn.h"
#include "webfuse_provider/impl/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_timer;

struct wfp_jsonrpc_request
{
    bool is_pending;
    wfp_jsonrpc_proxy_finished_fn * finished;
    void * user_data;
    int id;
    struct wfp_timer * timer;
};

struct wfp_jsonrpc_proxy
{
    struct wfp_jsonrpc_request request;
    int timeout;
    wfp_jsonrpc_send_fn * send;
    void * user_data;
};

extern void 
wfp_jsonrpc_proxy_init(
    struct wfp_jsonrpc_proxy * proxy,
    struct wfp_timer_manager * manager,
    int timeout,
    wfp_jsonrpc_send_fn * send,
    void * user_data);

extern void 
wfp_jsonrpc_proxy_cleanup(
    struct wfp_jsonrpc_proxy * proxy);

extern void wfp_jsonrpc_proxy_vinvoke(
	struct wfp_jsonrpc_proxy * proxy,
	wfp_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args);

extern void wfp_jsonrpc_proxy_vnotify(
	struct wfp_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args);

#ifdef __cplusplus
}
#endif

#endif
