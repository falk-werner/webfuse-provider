#include <gtest/gtest.h>
#include "webfuse_provider/impl/message_queue.h"
#include "webfuse_provider/impl/message.h"
#include "webfuse_provider/impl/slist.h"

namespace
{

    struct wfp_slist_item * create_message(char const * content)
    {
        json_t * value = json_object();
        json_object_set_new(value, "content", json_string(content));
        struct wfp_message * message = wfp_message_create(value);

        json_decref(value);
        return &message->item;
    }

}

TEST(wfp_message_queue, cleanup_empty_list)
{
    struct wfp_slist queue;
    wfp_slist_init(&queue);

    wfp_message_queue_cleanup(&queue);
    ASSERT_TRUE(wfp_slist_empty(&queue));
}

TEST(wfp_message_queue, cleanup_one_element)
{
    struct wfp_slist queue;
    wfp_slist_init(&queue);

    wfp_slist_append(&queue, create_message("Hello"));

    wfp_message_queue_cleanup(&queue);
    ASSERT_TRUE(wfp_slist_empty(&queue));
}

TEST(wfp_message_queue, cleanup_multiple_element)
{
    struct wfp_slist queue;
    wfp_slist_init(&queue);

    wfp_slist_append(&queue, create_message("Hello"));
    wfp_slist_append(&queue, create_message("World"));
    wfp_slist_append(&queue, create_message("!"));

    wfp_message_queue_cleanup(&queue);
    ASSERT_TRUE(wfp_slist_empty(&queue));
}