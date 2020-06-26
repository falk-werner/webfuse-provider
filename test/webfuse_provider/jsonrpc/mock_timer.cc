#include "webfuse_provider/jsonrpc/mock_timer.hpp"
#include "webfuse_provider/test_util/wrap.hpp"

extern "C"
{
static wfp_jsonrpc_test::ITimer * wfp_jsonrpc_MockTimer = nullptr;

WFP_WRAP_FUNC0(wfp_jsonrpc_MockTimer, wfp_timer_manager *, wfp_timer_manager_create);
WFP_WRAP_FUNC1(wfp_jsonrpc_MockTimer, void, wfp_timer_manager_dispose, wfp_timer_manager *);
WFP_WRAP_FUNC1(wfp_jsonrpc_MockTimer, void, wfp_timer_manager_check, wfp_timer_manager *);

WFP_WRAP_FUNC3(wfp_jsonrpc_MockTimer, wfp_timer *, wfp_timer_create, wfp_timer_manager *, wfp_timer_on_timer_fn *, void *);
WFP_WRAP_FUNC1(wfp_jsonrpc_MockTimer, void, wfp_timer_dispose, wfp_timer *);
WFP_WRAP_FUNC2(wfp_jsonrpc_MockTimer, void, wfp_timer_start, wfp_timer *, int);
WFP_WRAP_FUNC1(wfp_jsonrpc_MockTimer, void, wfp_timer_cancel, wfp_timer *);

}

namespace wfp_jsonrpc_test
{
MockTimer::MockTimer()
{
    wfp_jsonrpc_MockTimer = this;
}

MockTimer::~MockTimer()
{
    wfp_jsonrpc_MockTimer = nullptr;
}

}