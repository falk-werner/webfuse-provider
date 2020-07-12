#include <gtest/gtest.h>
#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/status.h"
#include "webfuse_provider/test_util/json_doc.hpp"
#include "webfuse_provider/impl/json/node.h"
#include "webfuse_provider/impl/jsonrpc/error.h"

using webfuse_test::JsonDoc;

TEST(wfp_json_response, init_result)
{
    JsonDoc doc("{\"result\": 47, \"id\": 11}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response,doc.root());

    ASSERT_EQ(nullptr, response.error);
    ASSERT_TRUE(wfp_impl_json_is_int(response.result));
    ASSERT_EQ(47, wfp_impl_json_get_int(response.result));
    ASSERT_EQ(11, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}

TEST(wfp_json_response, init_error)
{
    JsonDoc doc("{\"error\": {\"code\": 42, \"message\": \"Don't Panic!\"}, \"id\": 23}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(42, response.error->code);
    ASSERT_STREQ("Don't Panic!", response.error->message);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}

TEST(wfp_json_response, init_fail_missing_result_and_error)
{
    JsonDoc doc("{\"id\": 12}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(12, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}

TEST(wfp_json_response, init_fail_missing_id)
{
    JsonDoc doc("{\"result\": 47}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(-1, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}

TEST(wfp_json_response, init_fail_wrong_id_type)
{
    JsonDoc doc("{\"result\": 47, \"id\": \"42\"}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(-1, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}

TEST(wfp_json_response, init_fail_error_missing_code)
{
    JsonDoc doc("{\"error\": {\"message\": \"Don't Panic!\"}, \"id\": 23}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WFP_BAD_FORMAT,response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}

TEST(wfp_json_response, init_fail_error_wrong_code_type)
{
    JsonDoc doc("{\"error\": {\"code\": \"42\", \"message\": \"Don't Panic!\"}, \"id\": 23}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}

TEST(wfp_json_response, init_fail_error_wrong_type)
{
    JsonDoc doc("{\"error\": \"invalid error type\", \"id\": 23}");

    struct wfp_jsonrpc_response response;
    wfp_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WFP_BAD_FORMAT, response.error->code);
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wfp_jsonrpc_response_cleanup(&response);
}
