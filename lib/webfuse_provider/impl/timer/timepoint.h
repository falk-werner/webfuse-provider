#ifndef WFP_TIMER_TIMEPOINT_H
#define WFP_TIMER_TIMEPOINT_H

#ifndef __cplusplus
#include <stdbool.h>
#include <inttypes.h>
#else
#include <cinttypes>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint64_t wfp_timer_timepoint;
typedef int64_t wfp_timer_timediff;

extern wfp_timer_timepoint wfp_timer_timepoint_now(void);

extern wfp_timer_timepoint wfp_timer_timepoint_in_msec(
    wfp_timer_timediff value);

extern bool wfp_timer_timepoint_is_elapsed(
    wfp_timer_timepoint timepoint);

#ifdef __cplusplus
} 
#endif

#endif
