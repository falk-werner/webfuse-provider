#include "webfuse_provider/impl/json/parser.h"
#include "webfuse_provider/impl/json/node.h"
#include <gtest/gtest.h>
#include <cstring>

namespace
{
    wfp_json_doc * parse_json(char * text)
    {
        return wfp_impl_json_parse_buffer(text, strlen(text));
    }
}

TEST(json_parser, parse_true)
{
    char text[] = "true";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_bool(root));
    ASSERT_TRUE(wfp_impl_json_get_bool(root));

    wfp_impl_json_dispose(doc);
}

TEST(json_parser, parse_false)
{
    char text[] = "false";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_bool(root));
    ASSERT_FALSE(wfp_impl_json_get_bool(root));

    wfp_impl_json_dispose(doc);
}

TEST(json_parser, parse_int)
{
    char text[] = "42";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_int(root));
    ASSERT_EQ(42, wfp_impl_json_get_int(root));

    wfp_impl_json_dispose(doc);
}

TEST(json_parser, parse_negative_int)
{
    char text[] = "-1234";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_int(root));
    ASSERT_EQ(-1234, wfp_impl_json_get_int(root));

    wfp_impl_json_dispose(doc);
}

TEST(json_parser, parse_string)
{
    char text[] = "\"brummni\"";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_string(root));
    ASSERT_STREQ("brummni", wfp_impl_json_get_string(root));

    wfp_impl_json_dispose(doc);
}

TEST(json_parser, parse_array)
{
    char text[] = "[true,1,\"foo\",[42]]";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_array(root));
    ASSERT_EQ(4, wfp_impl_json_array_size(root));

    ASSERT_TRUE(wfp_impl_json_is_bool(wfp_impl_json_array_get(root, 0)));
    ASSERT_TRUE(wfp_impl_json_is_int(wfp_impl_json_array_get(root, 1)));
    ASSERT_TRUE(wfp_impl_json_is_string(wfp_impl_json_array_get(root, 2)));
    ASSERT_TRUE(wfp_impl_json_is_array(wfp_impl_json_array_get(root, 3)));

    wfp_impl_json_dispose(doc);
}

TEST(json_parser, parse_object)
{
    char text[] = "{\"method\":\"add\",\"params\":[1,2],\"id\":42}";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_object(root));
    ASSERT_EQ(3, wfp_impl_json_object_size(root));

    ASSERT_STREQ("method", wfp_impl_json_object_key(root, 0));
    ASSERT_STREQ("params", wfp_impl_json_object_key(root, 1));
    ASSERT_STREQ("id", wfp_impl_json_object_key(root, 2));

    wfp_impl_json_dispose(doc);
}

TEST(json_parser, parse_fail_invalid_json)
{
    {
        char text[] = "True";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "flas";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "[1,2,3}";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "{\"method\":\"add\",\"params\":[1,2],\"id\":42";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

}