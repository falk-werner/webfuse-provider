#ifndef WFP_TIMER_MANAGER_H
#define WFP_TIMER_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_timer_manager;

extern struct wfp_timer_manager * 
wfp_timer_manager_create(void);

extern void
wfp_timer_manager_dispose(
    struct wfp_timer_manager * manager);

extern void
wfp_timer_manager_check(
    struct wfp_timer_manager * manager);

#ifdef __cplusplus
}
#endif


#endif
