#ifndef WFP_TIMER_ON_TIMER_FN_H
#define WFP_TIMER_ON_TIMER_FN_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_timer;

typedef void wfp_timer_on_timer_fn(
    struct wfp_timer * timer,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
