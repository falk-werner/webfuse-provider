#include "webfuse_provider/impl/util/slist.h"
#include <stddef.h>

void wfp_slist_init(
    struct wfp_slist * list)
{
    list->head.next = NULL;
    list->last = &list->head;
}

bool wfp_slist_empty(
    struct wfp_slist * list)
{
    return (list->last == &list->head);
}

struct wfp_slist_item * wfp_slist_first(
    struct wfp_slist * list)
{
    return list->head.next;
}

void wfp_slist_append(
    struct wfp_slist * list,
    struct wfp_slist_item * item)
{
    item->next = NULL;
    list->last->next = item;
    list->last = item;
}

struct wfp_slist_item * wfp_slist_remove_first(
    struct wfp_slist * list)
{
    return wfp_slist_remove_after(list, &list->head);
}

struct wfp_slist_item * wfp_slist_remove_after(
    struct wfp_slist * list,
    struct wfp_slist_item * prev)
{

    struct wfp_slist_item * result = prev->next;

    if (NULL != result)
    {
        prev->next = result->next;

        if (list->last == result)
        {
            list->last = prev;
        }    
    }

    return result;
}
