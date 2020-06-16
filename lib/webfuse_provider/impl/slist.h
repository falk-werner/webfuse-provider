#ifndef WFP_SLIST_H
#define WFP_SLIST_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{ 
#endif

struct wfp_slist_item
{
    struct wfp_slist_item * next;
};

struct wfp_slist
{
    struct wfp_slist_item head;
    struct wfp_slist_item * last;
};

extern void wfp_slist_init(
    struct wfp_slist * list);

extern bool wfp_slist_empty(
    struct wfp_slist * list);

extern struct wfp_slist_item * wfp_slist_first(
    struct wfp_slist * list);

extern void wfp_slist_append(
    struct wfp_slist * list,
    struct wfp_slist_item * item);

extern struct wfp_slist_item * wfp_slist_remove_first(
    struct wfp_slist * list);

extern struct wfp_slist_item * wfp_slist_remove_after(
    struct wfp_slist * list,
    struct wfp_slist_item * prev);

#ifdef __cplusplus
}
#endif

#endif
