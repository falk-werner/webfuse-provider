#ifndef WFP_TIMER_MANAGER_INTERN_H
#define WFP_TIMER_MANAGER_INTERN_H

#include "webfuse_provider/impl/timer/manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_timer;

extern void wfp_timer_manager_addtimer(
    struct wfp_timer_manager * manager,
    struct wfp_timer * timer);

extern void wfp_timer_manager_removetimer(
    struct wfp_timer_manager * manager,
    struct wfp_timer * timer);

#ifdef __cplusplus
}
#endif


#endif
