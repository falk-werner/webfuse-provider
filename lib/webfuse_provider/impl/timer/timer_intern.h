#ifndef WFP_TIMER_TIMER_H
#define WFP_TIMER_TIMER_H

#include "webfuse_provider/impl/timer/timer.h"
#include "webfuse_provider/impl/timer/on_timer_fn.h"
#include "webfuse_provider/impl/timer/timepoint.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_timer
{
    struct wfp_timer_manager * manager;
    wfp_timer_timepoint timeout;
    wfp_timer_on_timer_fn * on_timer;
    void * user_data;
    struct wfp_timer * next;
    struct wfp_timer * prev;
};

extern bool wfp_timer_is_timeout(
    struct wfp_timer * timer);

extern void wfp_timer_trigger(
    struct wfp_timer * timer);


#ifdef __cplusplus
}
#endif



#endif
