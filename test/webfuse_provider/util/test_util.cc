#include <gtest/gtest.h>
#include "webfuse_provider/impl/util/json_util.h"

TEST(jsonrpc_util, get_int)
{
    json_t * object = json_object();
    json_object_set_new(object, "key", json_integer(23));
    int value = wfp_impl_json_get_integer(object, "key", 42);
    ASSERT_EQ(23, value);

    json_decref(object);
}

TEST(jsonrpc_util, failed_to_get_null_object)
{
    int value = wfp_impl_json_get_integer(nullptr, "key", 42);
    
    ASSERT_EQ(42, value);
}

TEST(jsonrpc_util, failed_to_get_not_object)
{
    json_t * object = json_array();
    int value = wfp_impl_json_get_integer(nullptr, "key", 42);    
    ASSERT_EQ(42, value);

    json_decref(object);
}

TEST(jsonrpc_util, failed_to_get_invalid_key)
{
    json_t * object = json_object();
    int value = wfp_impl_json_get_integer(object, "key", 42);
    ASSERT_EQ(42, value);

    json_decref(object);
}

TEST(jsonrpc_util, failed_to_get_invalid_value_type)
{
    json_t * object = json_object();
    json_object_set_new(object, "key", json_string("42"));
    int value = wfp_impl_json_get_integer(object, "key", 42);
    ASSERT_EQ(42, value);

    json_decref(object);
}

TEST(jsonrpc_util, get_status_good_if_no_error)
{
    json_t * error = nullptr;
    wfp_status status = wfp_impl_jsonrpc_get_status(error);
    ASSERT_EQ(WFP_GOOD, status);
}

TEST(jsonrpc_util, get_status)
{
    json_t * error = json_object();
    json_object_set_new(error, "code", json_integer(WFP_BAD_BUSY));
    wfp_status status = wfp_impl_jsonrpc_get_status(error);
    ASSERT_EQ(WFP_BAD_BUSY, status);
    json_decref(error);
}

TEST(jsonrpc_util, get_status_bad_format)
{
    json_t * error = json_array();
    json_array_append_new(error, json_integer(WFP_BAD_BUSY));
    wfp_status status = wfp_impl_jsonrpc_get_status(error);
    ASSERT_EQ(WFP_BAD_FORMAT, status);
    json_decref(error);
}
