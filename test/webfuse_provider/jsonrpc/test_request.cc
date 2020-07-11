#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/impl/json/parser.h"
#include <gtest/gtest.h>

TEST(wfp_jsonrpc_request, is_request_object_params)
{
    char text[] = "{\"method\": \"some_method\", \"params\": { }, \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_TRUE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_request, is_request_fail_missing_params)
{
    char text[] = "{\"method\": \"some_method\", \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_request, is_request_fail_params_wrong_type)
{
    char text[] = "{\"method\": \"some_method\", \"params\": \"invalid_params\", \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}
