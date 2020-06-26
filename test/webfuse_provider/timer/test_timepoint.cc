#include <gtest/gtest.h>

#include "webfuse_provider/impl/timer/timepoint.h"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

TEST(wfp_timer_timepoint, now)
{
    wfp_timer_timepoint start = wfp_timer_timepoint_now();
    std::this_thread::sleep_for(42ms);
    wfp_timer_timepoint end = wfp_timer_timepoint_now();

    ASSERT_LT(start, end);
    ASSERT_LT(end, start + 500);
}

TEST(wfp_timer_timepoint, in_msec)
{
    wfp_timer_timepoint now = wfp_timer_timepoint_now();
    wfp_timer_timepoint later = wfp_timer_timepoint_in_msec(42);

    ASSERT_LT(now, later);
    ASSERT_LT(later, now + 500);
}

TEST(wfp_timer_timepoint, elapsed)
{
    wfp_timer_timepoint now;
    
    now = wfp_timer_timepoint_now();
    ASSERT_TRUE(wfp_timer_timepoint_is_elapsed(now - 1));

    now = wfp_timer_timepoint_now();
    ASSERT_FALSE(wfp_timer_timepoint_is_elapsed(now + 500));
}
