#include <gtest/gtest.h>

#include <cstddef>
#include <chrono>
#include <thread>

#include "webfuse_provider/impl/timer/timer.h"
#include "webfuse_provider/impl/timer/manager.h"

using std::size_t;
using namespace std::chrono_literals;

extern "C"
{
    void on_timeout(struct wfp_timer * timer, void * user_data)
    {
        (void) timer;

        bool * triggered = reinterpret_cast<bool*>(user_data);
        *triggered = true;
    }
}

TEST(wfp_timer, init)
{
    bool triggered = false;
    struct wfp_timer_manager * manager = wfp_timer_manager_create();
    struct wfp_timer * timer = wfp_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wfp_timer_dispose(timer);
    wfp_timer_manager_dispose(manager);
}

TEST(wfp_timer, trigger)
{
    bool triggered = false;
    struct wfp_timer_manager * manager = wfp_timer_manager_create();
    struct wfp_timer * timer = wfp_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wfp_timer_start(timer, -1);
    wfp_timer_manager_check(manager);

    ASSERT_TRUE(triggered);

    wfp_timer_dispose(timer);
    wfp_timer_manager_dispose(manager);
}

TEST(wfp_timer, trigger_on_dispose)
{
    bool triggered = false;
    struct wfp_timer_manager * manager = wfp_timer_manager_create();
    struct wfp_timer * timer = wfp_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wfp_timer_start(timer, (5 * 60 * 1000));

    wfp_timer_manager_dispose(manager);
    ASSERT_TRUE(triggered);

    wfp_timer_dispose(timer);
}

TEST(wfp_timer, cancel)
{
    bool triggered = false;
    struct wfp_timer_manager * manager = wfp_timer_manager_create();
    struct wfp_timer * timer = wfp_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wfp_timer_start(timer, 250);
    std::this_thread::sleep_for(500ms);
    wfp_timer_cancel(timer);
    wfp_timer_manager_check(manager);

    ASSERT_FALSE(triggered);
    
    wfp_timer_dispose(timer);
    wfp_timer_manager_dispose(manager);
}

TEST(wfp_timer, cancel_multiple_timers)
{
    static size_t const count = 5;
    struct wfp_timer_manager * manager = wfp_timer_manager_create();
    struct wfp_timer * timer[count];

    bool triggered = false;
    for(size_t i = 0; i < count; i++)
    {
        timer[i] = wfp_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));
        wfp_timer_start(timer[i], 0);
    }

    std::this_thread::sleep_for(10ms);
    for(size_t i = 0; i < count; i++)
    {
        wfp_timer_cancel(timer[i]);
    }

    wfp_timer_manager_check(manager);
    ASSERT_FALSE(triggered);
    
    for(size_t i = 0; i < count; i++)
    {
        wfp_timer_dispose(timer[i]);
    }
    wfp_timer_manager_dispose(manager);
}

TEST(wfp_timer, multiple_timers)
{
    static size_t const count = 5;
    struct wfp_timer_manager * manager = wfp_timer_manager_create();
    struct wfp_timer * timer[count];
    bool triggered[count];

    for(size_t i = 0; i < count; i++)
    {
        timer[i] = wfp_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered[i]));
        triggered[i] = false;
        wfp_timer_start(timer[i], (300 - (50 * i)));
    }

    for(size_t i = 0; i < count; i++)
    {
        std::this_thread::sleep_for(100ms);
        wfp_timer_manager_check(manager);
    }

    for(size_t i = 0; i < count; i++)
    {
        ASSERT_TRUE(triggered[i]);    
        wfp_timer_dispose(timer[i]);
    }

    wfp_timer_manager_dispose(manager);
}

TEST(wfp_timer, dont_trigger_null_callback)
{
    struct wfp_timer_manager * manager = wfp_timer_manager_create();
    struct wfp_timer * timer = wfp_timer_create(manager, nullptr, nullptr);
    
    wfp_timer_start(timer, -1);
    wfp_timer_manager_check(manager);

    wfp_timer_dispose(timer);
    wfp_timer_manager_dispose(manager);
}
