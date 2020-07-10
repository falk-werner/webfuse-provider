#include <gtest/gtest.h>
#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/status.h"

namespace
{

struct Context
{
    json_t * response;
};

bool jsonrpc_send(
	json_t * request,
    void * user_data)
{
   Context * context = reinterpret_cast<Context*>(user_data);
   context->response = request;
   json_incref(request);

   return true;
}

}

TEST(wfp_jsonrpc_request, is_request_object_params)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("some_method"));
    json_object_set_new(request, "params", json_object());
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_TRUE(wfp_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wfp_jsonrpc_request, is_request_fail_missing_params)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("some_method"));
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_FALSE(wfp_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wfp_jsonrpc_request, is_request_fail_params_wrong_type)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("some_method"));
    json_object_set_new(request, "params", json_string("invalid_params"));
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_FALSE(wfp_jsonrpc_is_request(request));

    json_decref(request);
}
