#include <gtest/gtest.h>
#include "webfuse_provider/impl/jsonrpc/response.h"
#include "webfuse_provider/test_util/json_doc.hpp"

using webfuse_test::JsonDoc;

TEST(wfp_jsonrpc_is_response, valid_result)
{
    JsonDoc doc("{\"result\": {}, \"id\": 42}");

    ASSERT_TRUE(wfp_jsonrpc_is_response(doc.root()));
}

TEST(wfp_jsonrpc_is_response, valid_result_string)
{
    JsonDoc doc("{\"result\": \"also valid\", \"id\": 42}");

    ASSERT_TRUE(wfp_jsonrpc_is_response(doc.root()));
}

TEST(wfp_jsonrpc_is_response, valid_error)
{
    JsonDoc doc("{\"error\": { }, \"id\": 42}");

    ASSERT_TRUE(wfp_jsonrpc_is_response(doc.root()));
}

TEST(wfp_jsonrpc_is_response, invalid_null)
{
    ASSERT_FALSE(wfp_jsonrpc_is_response(nullptr));
}

TEST(wfp_jsonrpc_is_response, invalid_message)
{
    JsonDoc doc("[{ }, 42]");

    ASSERT_FALSE(wfp_jsonrpc_is_response(doc.root()));
}

TEST(wfp_jsonrpc_is_response, invalid_missing_id)
{
    JsonDoc doc("{\"result\": { } }");

    ASSERT_FALSE(wfp_jsonrpc_is_response(doc.root()));
}

TEST(wfp_jsonrpc_is_response, invalid_id_wrong_type)
{
    JsonDoc doc("{\"result\": { }, \"id\": \"42\"}");

    ASSERT_FALSE(wfp_jsonrpc_is_response(doc.root()));
}


TEST(wfp_jsonrpc_is_response, invalid_missing_result_and_error)
{
    JsonDoc doc("{\"id\": \"42\"}");

    ASSERT_FALSE(wfp_jsonrpc_is_response(doc.root()));
}

TEST(wfp_jsonrpc_is_response, invalid_error_wrong_type)
{
    JsonDoc doc("{\"error\": [], \"id\": \"42\"}");

    ASSERT_FALSE(wfp_jsonrpc_is_response(doc.root()));
}
