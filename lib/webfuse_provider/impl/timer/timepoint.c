#include "webfuse_provider/impl/timer/timepoint.h"

#include <time.h>

#define WFP_TIMER_MSEC_PER_SEC  ((wfp_timer_timepoint) 1000) 
#define WFP_TIMER_NSEC_PER_MSEC ((wfp_timer_timepoint) 1000 * 1000) 

wfp_timer_timepoint wfp_timer_timepoint_now(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    wfp_timer_timepoint const now = (tp.tv_sec * WFP_TIMER_MSEC_PER_SEC) + (tp.tv_nsec / WFP_TIMER_NSEC_PER_MSEC);
    return now;
}

wfp_timer_timepoint wfp_timer_timepoint_in_msec(wfp_timer_timediff value)
{
    wfp_timer_timepoint const now = wfp_timer_timepoint_now();
    wfp_timer_timepoint result = now + ((wfp_timer_timepoint) value);

    return result;
}

bool wfp_timer_timepoint_is_elapsed(wfp_timer_timepoint tp)
{
    wfp_timer_timepoint const now = wfp_timer_timepoint_now();
    wfp_timer_timediff const  diff = (wfp_timer_timediff) (tp - now);

    return (0 > diff);
}
