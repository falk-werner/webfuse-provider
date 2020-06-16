#include <gtest/gtest.h>
#include <cstring>
#include "webfuse_provider/impl/message.h"

TEST(wfp_message, create)
{
    json_t * value = json_object();
    
    struct wfp_message * message = wfp_message_create(value);
    ASSERT_NE(nullptr, message);
    ASSERT_EQ(2, message->length);
    ASSERT_TRUE(0 == strncmp("{}", message->data, 2));

    wfp_message_dispose(message);
    json_decref(value);
}

TEST(wfp_message, fail_to_create)
{
    struct wfp_message * message = wfp_message_create(nullptr);
    ASSERT_EQ(nullptr, message);
}