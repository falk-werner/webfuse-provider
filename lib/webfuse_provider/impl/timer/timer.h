#ifndef WFP_TIMER_TIMER_H
#define WFP_TIMER_TIMER_H

#include "webfuse_provider/impl/timer/on_timer_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_timer;
struct wfp_timer_manager;

extern struct wfp_timer *
wfp_timer_create(
    struct wfp_timer_manager * manager,
    wfp_timer_on_timer_fn * on_timer,
    void * user_data);

extern void
wfp_timer_dispose(
    struct wfp_timer * timer);

extern void
wfp_timer_start(
    struct wfp_timer * timer,
    int timeout_ms);

extern void
wfp_timer_cancel(
    struct wfp_timer * timer);

#ifdef __cplusplus
}
#endif

#endif
