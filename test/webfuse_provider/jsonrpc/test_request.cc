#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/test_util/json_doc.hpp"
#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

TEST(wfp_jsonrpc_request, is_request_object_params)
{
    JsonDoc doc("{\"method\": \"some_method\", \"params\": { }, \"id\": 42}");

    ASSERT_TRUE(wfp_jsonrpc_is_request(doc.root()));
}

TEST(wfp_jsonrpc_request, is_request_fail_missing_params)
{
    JsonDoc doc("{\"method\": \"some_method\", \"id\": 42}");

    ASSERT_FALSE(wfp_jsonrpc_is_request(doc.root()));
}

TEST(wfp_jsonrpc_request, is_request_fail_params_wrong_type)
{
    JsonDoc doc("{\"method\": \"some_method\", \"params\": \"invalid_params\", \"id\": 42}");

    ASSERT_FALSE(wfp_jsonrpc_is_request(doc.root()));
}
