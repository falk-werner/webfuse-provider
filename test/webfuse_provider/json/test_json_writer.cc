#include "webfuse_provider/impl/json/writer.h"
#include <gtest/gtest.h>
#include <string>
#include <climits>

namespace
{

std::string write_int(int value)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_write_int(writer, value);
    char * data = wfp_impl_json_writer_take_data(writer, nullptr);
    std::string result = data;

    wfp_impl_json_writer_dispose(writer);
    free(data);

    return result;    
}

}

TEST(json_writer, create_dispose)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_dispose(writer);
}

TEST(json_writer, write_int)
{

    ASSERT_EQ("0", write_int(0));
    
    ASSERT_EQ("5", write_int(5));
    ASSERT_EQ("23", write_int(23));
    ASSERT_EQ("42", write_int(42));

    char int_max[80];
    snprintf(int_max, 80, "%d", INT_MAX);
    ASSERT_EQ(int_max, write_int(INT_MAX));

    ASSERT_EQ("-1", write_int(-1));
    ASSERT_EQ("-69", write_int(-69));
    ASSERT_EQ("-1091", write_int(-1091));
    ASSERT_EQ("-123456", write_int(-123456));
    ASSERT_EQ("-2147483647", write_int(-2147483647));

    char int_min[80];
    snprintf(int_min, 80, "%d", INT_MIN);
    ASSERT_EQ(int_min, write_int(INT_MIN));

}

TEST(json_writer, write_empty_array)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_array_begin(writer);
    wfp_impl_json_writer_array_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("[]", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, write_one_element_array)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_array_begin(writer);
    wfp_impl_json_writer_write_int(writer, 42);
    wfp_impl_json_writer_array_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("[42]", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, write_int_array)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_array_begin(writer);
    wfp_impl_json_writer_write_int(writer, 1);
    wfp_impl_json_writer_write_int(writer, 2);
    wfp_impl_json_writer_write_int(writer, 3);
    wfp_impl_json_writer_array_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("[1,2,3]", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, write_nested_array)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_array_begin(writer);

    wfp_impl_json_writer_array_begin(writer);
    wfp_impl_json_writer_array_end(writer);

    wfp_impl_json_writer_array_begin(writer);
    wfp_impl_json_writer_write_int(writer, 1);
    wfp_impl_json_writer_array_end(writer);

    wfp_impl_json_writer_array_begin(writer);
    wfp_impl_json_writer_array_begin(writer);
    wfp_impl_json_writer_write_int(writer, 1);
    wfp_impl_json_writer_array_end(writer);
    wfp_impl_json_writer_array_end(writer);

    wfp_impl_json_writer_array_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("[[],[1],[[1]]]", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}


TEST(json_writer, write_empty_object)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_object_begin(writer);
    wfp_impl_json_writer_object_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("{}", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, write_one_element_object)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_object_begin(writer);
    wfp_impl_json_writer_object_write_int(writer, "answer", 42);
    wfp_impl_json_writer_object_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("{\"answer\":42}", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, write_mixed_object)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_object_begin(writer);
    
    wfp_impl_json_writer_object_write_int(writer, "a", 42);
    wfp_impl_json_writer_object_write_string(writer, "b", "0");    
    wfp_impl_json_writer_object_begin_array(writer, "c");
    wfp_impl_json_writer_array_end(writer);
    
    wfp_impl_json_writer_object_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("{\"a\":42,\"b\":\"0\",\"c\":[]}", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, write_nested_object)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_object_begin(writer);
    
    wfp_impl_json_writer_object_begin_object(writer, "a");
    wfp_impl_json_writer_object_begin_object(writer, "b");
    wfp_impl_json_writer_object_begin_object(writer, "c");
    wfp_impl_json_writer_object_end(writer);
    wfp_impl_json_writer_object_end(writer);
    wfp_impl_json_writer_object_end(writer);

    wfp_impl_json_writer_object_end(writer);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("{\"a\":{\"b\":{\"c\":{}}}}", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, escape_string)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_write_string(writer, "\"\\/\b\f\n\r\t");

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("\"\\\"\\\\/\\b\\f\\n\\r\\t\"", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, dont_escape_string_uncecked)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_write_string_nocheck(writer, "\"\\/\b\f\n\r\t");

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("\"\"\\/\b\f\n\r\t\"", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}


TEST(json_writer, write_bytes)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(128,0);
    wfp_impl_json_writer_write_bytes(writer, "1234", 4);

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("\"MTIzNA==\"", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}

TEST(json_writer, expand_buffer)
{
    wfp_json_writer * writer = wfp_impl_json_writer_create(1,0);
    wfp_impl_json_writer_write_string(writer, "very large contents");

    char * data = wfp_impl_json_writer_take_data(writer, nullptr);

    ASSERT_STREQ("\"very large contents\"", data);

    wfp_impl_json_writer_dispose(writer);
    free(data);
}