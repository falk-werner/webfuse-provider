#include "webfuse_provider/impl/timer/manager_intern.h"
#include "webfuse_provider/impl/timer/timer_intern.h"
#include "webfuse_provider/impl/timer/timepoint.h"

#include <stddef.h>
#include <stdlib.h>

struct wfp_timer_manager
{
    struct wfp_timer * timers;
};

struct wfp_timer_manager *
wfp_timer_manager_create(void)
{
    struct wfp_timer_manager * manager = malloc(sizeof(struct wfp_timer_manager));
    manager->timers = NULL;

    return manager;
}

void
wfp_timer_manager_dispose(
    struct wfp_timer_manager * manager)
{
    struct wfp_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wfp_timer * next = timer->next;

        wfp_timer_trigger(timer);
        timer = next;
    }

    free(manager);
}


void wfp_timer_manager_check(
    struct wfp_timer_manager * manager)
{
    struct wfp_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wfp_timer * next = timer->next;

        if (wfp_timer_is_timeout(timer))
        {
            wfp_timer_manager_removetimer(manager, timer);
            wfp_timer_trigger(timer);
        }

        timer = next;
    }    
}

void wfp_timer_manager_addtimer(
    struct wfp_timer_manager * manager,
    struct wfp_timer * timer)
{
    if (NULL != manager->timers)
    {
        manager->timers->prev = timer;
    }

    timer->next = manager->timers;
    timer->prev = NULL;
    manager->timers = timer;
}

void wfp_timer_manager_removetimer(
    struct wfp_timer_manager * manager,
    struct wfp_timer * timer)
{
    struct wfp_timer * prev = timer->prev;
    struct wfp_timer * next = timer->next;

    if (NULL != prev)
    {
        prev->next = next;
    }

    if (NULL != next)
    {
        next->prev = prev;
    }

    if (manager->timers == timer)
    {
        manager->timers = next;
    }
}

