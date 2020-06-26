#ifndef WFP_JSONRPC_MOCK_TIMERCALLBACK_HPP
#define WFP_JSONRPC_MOCK_TIMERCALLBACK_HPP

#include "webfuse_provider/impl/timer/on_timer_fn.h"
#include <gmock/gmock.h>

namespace wfp_jsonrpc_test
{
class MockTimerCallback
{
public:
    MockTimerCallback();
    virtual ~MockTimerCallback();
    wfp_timer_on_timer_fn * on_timer_fn();
    void * user_data();

    MOCK_METHOD2(on_timer, void (wfp_timer * timer, void * user_data));

};

}

#endif
