#include <gtest/gtest.h>
#include "webfuse_provider/impl/message_queue.h"
#include "webfuse_provider/impl/message.h"
#include "webfuse_provider/impl/util/slist.h"

#include <libwebsockets.h>
#include <cstdlib>
#include <cstring>

namespace
{

    struct wfp_slist_item * create_message(char const * content)
    {
        size_t length = strlen(content);
        char * raw_data = reinterpret_cast<char*>(malloc(LWS_PRE + length));
        char * data = raw_data + LWS_PRE;

        memcpy(data, content, length);
        struct wfp_message * message = wfp_message_create(data, length);

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