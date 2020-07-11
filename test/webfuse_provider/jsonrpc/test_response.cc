#include <gtest/gtest.h>
#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/status.h"
#include "webfuse_provider/impl/json/parser.h"
#include "webfuse_provider/impl/json/node.h"
#include "webfuse_provider/impl/jsonrpc/error.h"

TEST(wfp_json_response, init_result)
{
    char text[] = "{\"result\": 47, \"id\": 11}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(nullptr, response.error);
    ASSERT_TRUE(wfp_impl_json_is_int(response.result));
    ASSERT_EQ(47, wfp_impl_json_get_int(response.result));
    ASSERT_EQ(11, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}

TEST(wfp_json_response, init_error)
{
    char text[] = "{\"error\": {\"code\": 42, \"message\": \"Don't Panic!\"}, \"id\": 23}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(42, response.error->code);
    ASSERT_STREQ("Don't Panic!", response.error->message);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}

TEST(wfp_json_response, init_fail_missing_result_and_error)
{
    char text[] = "{\"id\": 12}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(12, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}

TEST(wfp_json_response, init_fail_missing_id)
{
    char text[] = "{\"result\": 47}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(-1, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}

TEST(wfp_json_response, init_fail_wrong_id_type)
{
    char text[] = "{\"result\": 47, \"id\": \"42\"}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(-1, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}

TEST(wfp_json_response, init_fail_error_missing_code)
{
    char text[] = "{\"error\": {\"message\": \"Don't Panic!\"}, \"id\": 23}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(WFP_BAD_FORMAT,response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}

TEST(wfp_json_response, init_fail_error_wrong_code_type)
{
    char text[] = "{\"error\": {\"code\": \"42\", \"message\": \"Don't Panic!\"}, \"id\": 23}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}

TEST(wfp_json_response, init_fail_error_wrong_type)
{
    char text[] = "{\"error\": \"invalid error type\", \"id\": 23}";
    wfp_json_doc * doc = wfp_impl_json_parse(text);

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
    wfp_impl_json_dispose(doc);
}
