#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/impl/json/parser.h"

#include <gtest/gtest.h>

TEST(wfp_jsonrpc_is_request, request_with_object_params)
{
    char text[] = "{\"method\": \"method\", \"params\": {}, \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_TRUE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, request_with_array_params)
{
    char text[] = "{\"method\": \"method\", \"params\": [], \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    ASSERT_TRUE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, null_request)
{
    ASSERT_FALSE(wfp_jsonrpc_is_request(nullptr));
}

TEST(wfp_jsonrpc_is_request, invalid_request)
{
    char text[] = "[\"method\", { }, 42]";
    wfp_json_doc * doc = wfp_impl_json_parse(text);
    
    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, invalid_request_without_id)
{
    char text[] = "{\"method\": \"method\", \"params\": { }}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);
    
    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, invalid_request_due_to_invalid_id)
{
    char text[] = "{\"method\": \"method\", \"params\": { }, \"id\": \"42\"}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);
    
    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, invalid_request_without_method)
{
    char text[] = "{\"params\": { }, \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);
    
    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, invalid_request_due_to_invalid_method)
{
    char text[] = "{\"method\": 42, \"params\": {}, \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);
    
    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, invalid_request_without_params)
{
    char text[] = "{\"method\": \"method\", \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);
    
    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}

TEST(wfp_jsonrpc_is_request, invalid_request_due_to_invalid_params)
{
    char text[] = "{\"method\": \"method\", \"params\": \"params\", \"id\": 42}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);
    
    ASSERT_FALSE(wfp_jsonrpc_is_request(wfp_impl_json_root(doc)));

    wfp_impl_json_dispose(doc);
}
