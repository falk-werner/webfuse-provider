#ifndef WFP_JSONRPC_PROXY_FINISHED_FN_H
#define WFP_JSONRPC_PROXY_FINISHED_FN_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json;
struct wfp_jsonrpc_error;

typedef void wfp_jsonrpc_proxy_finished_fn(
	void * user_data,
	struct wfp_json const * result,
    struct wfp_jsonrpc_error const * error);

#ifdef __cplusplus
}
#endif

#endif
