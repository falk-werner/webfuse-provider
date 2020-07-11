#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/impl/jsonrpc/error.h"
#include "webfuse_provider/impl/json/parser.h"

#include <gtest/gtest.h>


TEST(response_parser, fail_no_object)
{
	char text[] = "[]";
	wfp_json_doc * doc = wfp_impl_json_parse(text);
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
	wfp_impl_json_dispose(doc);
}

TEST(response_error, fail_empty_object)
{
	char text[] = "{}";
	wfp_json_doc * doc = wfp_impl_json_parse(text);
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
	wfp_impl_json_dispose(doc);
}

TEST(response_error, fail_no_data)
{
	char text[] = "{\"id\":42}";
	wfp_json_doc * doc = wfp_impl_json_parse(text);
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
	wfp_impl_json_dispose(doc);
}

TEST(response_error, fail_with_custom_error_code)
{
	char text[] = "{\"error\":{\"code\": 42}, \"id\": 42}";
	wfp_json_doc * doc = wfp_impl_json_parse(text);
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, response.error->code);
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
	wfp_impl_json_dispose(doc);
}

TEST(response_parser, fail_invalid_response)
{
	char text[] = "{\"result\": true, \"id\": 42}";
	wfp_json_doc * doc = wfp_impl_json_parse(text);
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, wfp_impl_json_root(doc));

	ASSERT_EQ(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_NE(nullptr, response.result);

	wfp_jsonrpc_response_cleanup(&response);
	wfp_impl_json_dispose(doc);
}
