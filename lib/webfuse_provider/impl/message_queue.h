#ifndef WFP_MESSAGE_QUEUE_H
#define WFP_MESSAGE_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_slist;

extern void wfp_message_queue_cleanup(
    struct wfp_slist * queue);


#ifdef __cplusplus
}
#endif

#endif
