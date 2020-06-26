#include "webfuse_provider/impl/jsonrpc/proxy_intern.h"

void wfp_jsonrpc_proxy_invoke(
	struct wfp_jsonrpc_proxy * proxy,
	wfp_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	...)
{
    va_list args;
    va_start(args, param_info);
    wfp_jsonrpc_proxy_vinvoke(proxy, finished, user_data, method_name, param_info, args);
    va_end(args);
}

extern void wfp_jsonrpc_proxy_notify(
	struct wfp_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	...
)
{
    va_list args;
    va_start(args, param_info);
    wfp_jsonrpc_proxy_vnotify(proxy, method_name, param_info, args);
    va_end(args);
}
