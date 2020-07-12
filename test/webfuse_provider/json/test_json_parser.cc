#include "webfuse_provider/impl/json/doc.h"
#include "webfuse_provider/impl/json/node.h"
#include <gtest/gtest.h>
#include <cstring>

namespace
{
    wfp_json_doc * parse_json(char * text)
    {
        return wfp_impl_json_doc_loadb(text, strlen(text));
    }
}

TEST(json_parser, parse_null)
{
    char text[] = "null";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_null(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_true)
{
    char text[] = "true";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_bool(root));
    ASSERT_TRUE(wfp_impl_json_bool_get(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_false)
{
    char text[] = "false";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_bool(root));
    ASSERT_FALSE(wfp_impl_json_bool_get(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_int)
{
    char text[] = "42";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_int(root));
    ASSERT_EQ(42, wfp_impl_json_int_get(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_negative_int)
{
    char text[] = "-1234";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_int(root));
    ASSERT_EQ(-1234, wfp_impl_json_int_get(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_string)
{
    char text[] = "\"brummni\"";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_string(root));
    ASSERT_STREQ("brummni", wfp_impl_json_string_get(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, unescape_string)
{
    char text[] = "\"\\\"_\\\\_\\/_\\b_\\f_\\n_\\r_\\t\"";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_string(root));
    ASSERT_STREQ("\"_\\_/_\b_\f_\n_\r_\t", wfp_impl_json_string_get(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, skip_whitespace)
{
    char text[] = " \t\r\n\"42\"";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_string(root));
    ASSERT_STREQ("42", wfp_impl_json_string_get(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_array)
{
    char text[] = "[true,1,\"foo\",[42]]";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_array(root));
    ASSERT_EQ(4, wfp_impl_json_array_size(root));

    ASSERT_TRUE(wfp_impl_json_is_bool(wfp_impl_json_array_get(root, 0)));
    ASSERT_TRUE(wfp_impl_json_is_int(wfp_impl_json_array_get(root, 1)));
    ASSERT_TRUE(wfp_impl_json_is_string(wfp_impl_json_array_get(root, 2)));
    ASSERT_TRUE(wfp_impl_json_is_array(wfp_impl_json_array_get(root, 3)));
    ASSERT_TRUE(wfp_impl_json_is_null(wfp_impl_json_array_get(root, 4)));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_empty_array)
{
    char text[] = "[]";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_array(root));
    ASSERT_EQ(0, wfp_impl_json_array_size(root));

    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, parse_object)
{
    char text[] = "{\"method\":\"add\",\"params\":[1,2],\"id\":42}";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);
    wfp_json const * root = wfp_impl_json_doc_root(doc);
    ASSERT_TRUE(wfp_impl_json_is_object(root));
    ASSERT_EQ(3, wfp_impl_json_object_size(root));

    ASSERT_STREQ("method", wfp_impl_json_object_key(root, 0));
    ASSERT_TRUE(wfp_impl_json_is_string(wfp_impl_json_object_value(root, 0)));

    ASSERT_STREQ("params", wfp_impl_json_object_key(root, 1));
    ASSERT_TRUE(wfp_impl_json_is_array(wfp_impl_json_object_value(root, 1)));

    ASSERT_STREQ("id", wfp_impl_json_object_key(root, 2));
    ASSERT_TRUE(wfp_impl_json_is_int(wfp_impl_json_object_value(root, 2)));

    ASSERT_STREQ("", wfp_impl_json_object_key(root, 3));
    ASSERT_TRUE(wfp_impl_json_is_null(wfp_impl_json_object_value(root, 3)));


    wfp_impl_json_doc_dispose(doc);
}

TEST(json_parser, default_values)
{
    char text[] = "[true]";
    wfp_json_doc * doc = parse_json(text);
    ASSERT_NE(nullptr, doc);

    wfp_json const * json_array = wfp_impl_json_doc_root(doc);
    ASSERT_FALSE(wfp_impl_json_bool_get(json_array));
    ASSERT_EQ(0, wfp_impl_json_int_get(json_array));
    ASSERT_STREQ("", wfp_impl_json_string_get(json_array));
    ASSERT_EQ(0, wfp_impl_json_object_size(json_array));
    ASSERT_TRUE(wfp_impl_json_is_null(wfp_impl_json_object_get(json_array, "foo")));
    ASSERT_STREQ("", wfp_impl_json_object_key(json_array, 0));
    ASSERT_TRUE(wfp_impl_json_is_null(wfp_impl_json_object_value(json_array, 0)));

    wfp_json const * json_bool = wfp_impl_json_array_get(json_array, 0);
    ASSERT_EQ(0, wfp_impl_json_array_size(json_bool));
    ASSERT_TRUE(wfp_impl_json_is_null(wfp_impl_json_array_get(json_bool, 0)));

    wfp_impl_json_doc_dispose(doc);

}

TEST(json_parser, parse_fail_invalid_json)
{
    {
        char text[] = "";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "invalid";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "nul";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "tru";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "flas";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "+1";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "\"unterminated_string";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "\"invale_\\escape_\\sequence\"";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "[1,2,3}";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "[1 2 3]";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "[1,2,3";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "[1,2,]";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "{\"method\":\"add\",\"params\":[1,2],\"id\":42";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "[\"method\",[], {}, \"params\":,42]";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "{\"key\" \"value\"}";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "{\"key\": }";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "{\"key\": \"value\"";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "{\"key\" \"value\"]";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

    {
        char text[] = "{\"key\": \"value\", }";
        wfp_json_doc * doc = parse_json(text);
        ASSERT_EQ(nullptr, doc);
    }

}