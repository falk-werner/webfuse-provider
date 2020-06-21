#include "webfuse_provider/mocks/mock_jsonrpc_proxy.hpp"
#include "webfuse_provider/test_util/wrap.hpp"

extern "C"
{
static webfuse_test::MockJsonRpcProxy * webfuse_test_MockJsonRpcProxy = nullptr;

WFP_WRAP_VFUNC5(webfuse_test_MockJsonRpcProxy, void, wfp_jsonrpc_proxy_vinvoke,
	struct wfp_jsonrpc_proxy *,
	wfp_jsonrpc_proxy_finished_fn *,
	void *,
	char const *,
	char const *);

WFP_WRAP_VFUNC3(webfuse_test_MockJsonRpcProxy, void, wfp_jsonrpc_proxy_vnotify,
	struct wfp_jsonrpc_proxy *,
	char const *,
	char const *);
}

namespace webfuse_test
{

MockJsonRpcProxy::MockJsonRpcProxy()
{
    webfuse_test_MockJsonRpcProxy = this;
}

MockJsonRpcProxy::~MockJsonRpcProxy()
{
    webfuse_test_MockJsonRpcProxy = nullptr;
}

}