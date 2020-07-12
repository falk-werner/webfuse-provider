#include "webfuse_provider/impl/message.h"

#include <gtest/gtest.h>
#include <libwebsockets.h>

#include <cstdlib>
#include <cstring>

TEST(wfp_message, create)
{
    size_t length = 3;
    char * raw_data = reinterpret_cast<char*>(malloc(LWS_PRE + length));
    char * data = raw_data + LWS_PRE;
    snprintf(data, length, "{}");

    struct wfp_message * message = wfp_message_create(data, length);
    ASSERT_NE(nullptr, message);
    ASSERT_EQ(3, message->length);
    ASSERT_STREQ("{}", message->data);

    wfp_message_dispose(message);
}
