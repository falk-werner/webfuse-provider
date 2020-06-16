#ifndef WFP_JSONRPC_MOCK_TIMERMANAGER_HPP
#define WFP_JSONRPC_MOCK_TIMERMANAGER_HPP

#include "webfuse_provider/impl/timer/timer.h"
#include "webfuse_provider/impl/timer/manager.h"
#include <gmock/gmock.h>

namespace wfp_jsonrpc_test
{

class ITimer
{
public:
    virtual ~ITimer() = default;
    virtual wfp_timer_manager * wfp_timer_manager_create() = 0;
    virtual void wfp_timer_manager_dispose(wfp_timer_manager * manager) = 0;
    virtual void wfp_timer_manager_check(wfp_timer_manager * manager) = 0;
    virtual wfp_timer * wfp_timer_create(
        wfp_timer_manager * manager,
        wfp_timer_on_timer_fn * on_timer,
        void * user_data) = 0;
    virtual void wfp_timer_dispose(wfp_timer * timer) = 0;
    virtual void wfp_timer_start(wfp_timer * timer, int timeout_ms) = 0;
   virtual void wfp_timer_cancel(wfp_timer * timer) = 0; 
};

class MockTimer: public ITimer
{
public:
    MockTimer();
    ~MockTimer() override;
    MOCK_METHOD0(wfp_timer_manager_create, wfp_timer_manager * ());
    MOCK_METHOD1(wfp_timer_manager_dispose, void(wfp_timer_manager * manager));
    MOCK_METHOD1(wfp_timer_manager_check, void (wfp_timer_manager * manager));
    MOCK_METHOD3(wfp_timer_create,  wfp_timer *(
        wfp_timer_manager * manager,
        wfp_timer_on_timer_fn * on_timer,
        void * user_data));
    MOCK_METHOD1(wfp_timer_dispose, void (wfp_timer * timer));
    MOCK_METHOD2(wfp_timer_start, void (wfp_timer * timer, int timeout_ms));
    MOCK_METHOD1(wfp_timer_cancel, void (wfp_timer * timer)); 

};


}

#endif
