#include "webfuse_provider/mocks/mock_operation_context.hpp"
#include "webfuse/test_util/wrap.hpp"

extern "C"
{
static webfuse_test::MockOperationContext * webfuse_test_MockOperationContext = nullptr;

WFP_WRAP_FUNC1(webfuse_test_MockOperationContext, 
    struct wfp_jsonrpc_proxy *,  wfp_impl_operation_context_get_proxy,
	struct wfp_impl_operation_context *);

}

namespace webfuse_test
{

MockOperationContext::MockOperationContext()
{
    webfuse_test_MockOperationContext = this;
}

MockOperationContext::~MockOperationContext()
{
    webfuse_test_MockOperationContext = nullptr;
}

}