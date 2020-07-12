#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/impl/jsonrpc/error.h"
#include "webfuse_provider/test_util/json_doc.hpp"

#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

TEST(response_parser, fail_no_object)
{
	JsonDoc doc("[]");
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
}

TEST(response_error, fail_empty_object)
{
	JsonDoc doc("{}");
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
}

TEST(response_error, fail_no_data)
{
	JsonDoc doc("{\"id\":42}");
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
}

TEST(response_error, fail_with_custom_error_code)
{
	JsonDoc doc("{\"error\":{\"code\": 42}, \"id\": 42}");
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, response.error->code);
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
}

TEST(response_parser, fail_invalid_response)
{
	JsonDoc doc("{\"result\": true, \"id\": 42}");
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, doc.root());

	ASSERT_EQ(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_NE(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
}
