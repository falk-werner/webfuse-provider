#include <gtest/gtest.h>
#include "webfuse_provider/impl/jsonrpc/response.h"
#include "webfuse_provider/impl/json/parser.h"

TEST(wfp_jsonrpc_is_response, valid_result)
{
    char text[] = "{\"result\": {}, \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_TRUE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_response, valid_result_string)
{
    char text[] = "{\"result\": \"also valid\", \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_TRUE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_response, valid_error)
{
    char text[] = "{\"error\": { }, \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_TRUE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_response, invalid_null)
{
    ASSERT_FALSE(wfp_jsonrpc_is_response(nullptr));
}

TEST(wfp_jsonrpc_is_response, invalid_message)
{
    char text[] = "[{ }, 42]";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_FALSE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_response, invalid_missing_id)
{
    char text[] = "{\"result\": { } }";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_FALSE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_response, invalid_id_wrong_type)
{
    char text[] = "{\"result\": { }, \"id\": \"42\"}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_FALSE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}


TEST(wfp_jsonrpc_is_response, invalid_missing_result_and_error)
{
    char text[] = "{\"id\": \"42\"}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_FALSE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_response, invalid_error_wrong_type)
{
    char text[] = "{\"error\": [], \"id\": \"42\"}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_FALSE(wfp_jsonrpc_is_response(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}
