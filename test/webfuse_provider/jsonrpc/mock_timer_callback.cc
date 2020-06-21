#include "webfuse_provider/jsonrpc/mock_timer_callback.hpp"

extern "C"
{
using wfp_jsonrpc_test::MockTimerCallback;

static void wfp_jsonrpc_test_MockTimerCallback_on_timer(
    wfp_timer * timer,
    void * user_data)
{
    auto * self = reinterpret_cast<MockTimerCallback*>(user_data);
    self->on_timer(timer, user_data);
}

}

namespace wfp_jsonrpc_test
{

MockTimerCallback::MockTimerCallback()
{

}

MockTimerCallback::~MockTimerCallback()
{

}

wfp_timer_on_timer_fn * MockTimerCallback::on_timer_fn()
{
    return &wfp_jsonrpc_test_MockTimerCallback_on_timer;
}

void * MockTimerCallback::user_data()
{
    return reinterpret_cast<void*>(this);
}


}