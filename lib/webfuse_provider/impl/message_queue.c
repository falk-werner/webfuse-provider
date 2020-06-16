#include "webfuse_provider/impl/message_queue.h"
#include "webfuse_provider/impl/message.h"
#include "webfuse_provider/impl/container_of.h"

void wfp_message_queue_cleanup(
    struct wfp_slist * queue)
{
    struct wfp_slist_item * item = wfp_slist_first(queue);
    while (NULL != item)
    {
        struct wfp_slist_item * next = item->next;
        struct wfp_message * message = wfp_container_of(item, struct wfp_message, item);
        wfp_message_dispose(message);
        item = next;
    }
    wfp_slist_init(queue);
}
