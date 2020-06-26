#include "webfuse_provider/impl/timer/timer_intern.h"
#include "webfuse_provider/impl/timer/manager_intern.h"
#include "webfuse_provider/impl/timer/timepoint.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct wfp_timer *
wfp_timer_create(
    struct wfp_timer_manager * manager,
    wfp_timer_on_timer_fn * on_timer,
    void * user_data)
{
    struct wfp_timer * timer = malloc(sizeof(struct wfp_timer));
    timer->manager = manager;
    timer->timeout = 0;
    timer->on_timer = on_timer;
    timer->user_data = user_data;
    timer->prev = NULL;
    timer->next = NULL;

    return timer;
}

void
wfp_timer_dispose(
    struct wfp_timer * timer)
{
    free(timer);
}

void wfp_timer_start(
    struct wfp_timer * timer,
    int timeout_ms)
{
    timer->timeout = wfp_timer_timepoint_in_msec(timeout_ms);

    wfp_timer_manager_addtimer(timer->manager, timer);
}

void wfp_timer_cancel(
    struct wfp_timer * timer)
{
    wfp_timer_manager_removetimer(timer->manager, timer);

    timer->timeout = 0;
}

bool wfp_timer_is_timeout(
    struct wfp_timer * timer)
{
    return wfp_timer_timepoint_is_elapsed(timer->timeout);
}


void wfp_timer_trigger(
    struct wfp_timer * timer)
{
    if (0 != timer->on_timer)
    {
        timer->prev = NULL;
        timer->next = NULL;

        timer->on_timer(timer, timer->user_data);
    }
}
