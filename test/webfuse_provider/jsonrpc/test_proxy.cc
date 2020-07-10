#include <gtest/gtest.h>
#include "webfuse_provider/impl/jsonrpc/proxy.h"
#include "webfuse_provider/status.h"
#include "webfuse_provider/impl/timer/manager.h"

#include "webfuse_provider/jsonrpc/mock_timer.hpp"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using wfp_jsonrpc_test::MockTimer;
using testing::Return;
using testing::_;
using testing::DoAll;
using testing::SaveArg;

#define WFP_DEFAULT_TIMEOUT (10 * 1000)

namespace
{
    int jsonrpc_get_status(json_t * error)
    {
        json_t * code = json_object_get(error, "code");
        return (json_is_integer(code)) ? json_integer_value(code) : WFP_BAD_FORMAT;
    }

    struct SendContext
    {
        json_t * response;
        bool is_called;

        explicit SendContext()
        : response(nullptr)
        , is_called(false)
        {
        }

        ~SendContext() 
        {
            if (nullptr != response)
            {
                json_decref(response);
            }
        }
    };

    void jsonrpc_send(
        json_t * request,
        void * user_data)
    {
        SendContext * context = reinterpret_cast<SendContext*>(user_data);
        context->is_called = true;
        context->response = request;
        json_incref(request);
    }

    struct FinishedContext
    {
        bool is_called;
        json_t * result;
        json_t * error;

        FinishedContext()
        : is_called(false)
        , result(nullptr)
        , error(nullptr)
        {

        }

        ~FinishedContext()
        {
            if (nullptr != result)
            {
                json_decref(result);
            }

            if (nullptr != error)
            {
                json_decref(error);
            }
        }
    };

    void jsonrpc_finished(
        void * user_data,
        json_t const * result,
        json_t const * error)
    {
        FinishedContext * context = reinterpret_cast<FinishedContext*>(user_data);
        context->is_called = true;
        context->result = json_deep_copy(result);
        context->error = json_deep_copy(error);
    }
}

TEST(wfp_jsonrpc_proxy, init)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext context;
    void * user_data = reinterpret_cast<void*>(&context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, user_data);

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);

    ASSERT_FALSE(context.is_called);
}

TEST(wfp_jsonrpc_proxy, invoke)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * method = json_object_get(send_context.response, "method");
    ASSERT_TRUE(json_is_string(method));
    ASSERT_STREQ("foo", json_string_value(method));

    json_t * params = json_object_get(send_context.response, "params");
    ASSERT_TRUE(json_is_array(params));
    ASSERT_EQ(2, json_array_size(params));
    ASSERT_TRUE(json_is_string(json_array_get(params, 0)));
    ASSERT_STREQ("bar", json_string_value(json_array_get(params, 0)));
    ASSERT_TRUE(json_is_integer(json_array_get(params, 1)));
    ASSERT_EQ(42, json_integer_value(json_array_get(params, 1)));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_TRUE(json_is_integer(id));

    ASSERT_FALSE(finished_context.is_called);

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_FALSE(nullptr == finished_context.error);
}

TEST(wfp_jsonrpc_proxy, invoke_fails_if_another_request_is_pending)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    FinishedContext finished_context2;
    void * finished_data2 = reinterpret_cast<void*>(&finished_context2);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data2, "foo", "");

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    ASSERT_FALSE(finished_context.is_called);

    ASSERT_TRUE(finished_context2.is_called);
    ASSERT_EQ(WFP_BAD_BUSY, jsonrpc_get_status(finished_context2.error));

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

TEST(wfp_jsonrpc_proxy, invoke_invalid_request)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "?", "error");

    ASSERT_TRUE(send_context.is_called);

    ASSERT_FALSE(finished_context.is_called);

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

TEST(wfp_jsonrpc_proxy, on_result)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_TRUE(json_is_number(id));

    json_t * response = json_object();
    json_object_set_new(response, "result", json_string("okay"));
    json_object_set(response, "id", id);

    wfp_jsonrpc_proxy_onresult(proxy, response);
    json_decref(response);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_EQ(nullptr, finished_context.error);
    ASSERT_TRUE(json_is_string(finished_context.result));
    ASSERT_STREQ("okay", json_string_value(finished_context.result));

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

TEST(wfp_jsonrpc_proxy, on_result_reject_response_with_unknown_id)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_TRUE(json_is_number(id));

    json_t * response = json_object();
    json_object_set_new(response, "result", json_string("okay"));
    json_object_set_new(response, "id", json_integer(1 + json_integer_value(id)));

    wfp_jsonrpc_proxy_onresult(proxy, response);
    json_decref(response);

    ASSERT_FALSE(finished_context.is_called);

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

TEST(wfp_jsonrpc_proxy, timeout)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, 0, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    std::this_thread::sleep_for(10ms);
    wfp_timer_manager_check(timer_manager);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_EQ(WFP_BAD_TIMEOUT, jsonrpc_get_status(finished_context.error));

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

TEST(wfp_jsonrpc_proxy, cleanup_pending_request)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, 10, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wfp_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    ASSERT_FALSE(finished_context.is_called);

    wfp_jsonrpc_proxy_dispose(proxy);

    ASSERT_TRUE(finished_context.is_called);

    wfp_timer_manager_dispose(timer_manager);
}



TEST(wfp_jsonrpc_proxy, notify)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    wfp_jsonrpc_proxy_notify(proxy, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * method = json_object_get(send_context.response, "method");
    ASSERT_TRUE(json_is_string(method));
    ASSERT_STREQ("foo", json_string_value(method));

    json_t * params = json_object_get(send_context.response, "params");
    ASSERT_TRUE(json_is_array(params));
    ASSERT_EQ(2, json_array_size(params));
    ASSERT_TRUE(json_is_string(json_array_get(params, 0)));
    ASSERT_STREQ("bar", json_string_value(json_array_get(params, 0)));
    ASSERT_TRUE(json_is_integer(json_array_get(params, 1)));
    ASSERT_EQ(42, json_integer_value(json_array_get(params, 1)));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_EQ(nullptr, id);

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

TEST(wfp_jsonrpc_proxy, notify_send_invalid_request)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    wfp_jsonrpc_proxy_notify(proxy, "foo", "?");

    ASSERT_TRUE(send_context.is_called);

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

TEST(wfp_jsonrpc_proxy, swallow_timeout_if_no_request_pending)
{
    MockTimer timer_api;

    wfp_timer_on_timer_fn * on_timer = nullptr;
    void * timer_context = nullptr;
    EXPECT_CALL(timer_api, wfp_timer_create(_, _, _))
        .Times(1)
        .WillOnce(DoAll(SaveArg<1>(&on_timer), SaveArg<2>(&timer_context), Return(nullptr)));
    EXPECT_CALL(timer_api, wfp_timer_dispose(_)).Times(1);

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(nullptr, 1, &jsonrpc_send, send_data);

    on_timer(nullptr, timer_context);
    ASSERT_FALSE(send_context.is_called);


    wfp_jsonrpc_proxy_dispose(proxy);
}

TEST(wfp_jsonrpc_proxy, on_result_swallow_if_no_request_pending)
{
    struct wfp_timer_manager * timer_manager = wfp_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wfp_jsonrpc_proxy * proxy = wfp_jsonrpc_proxy_create(timer_manager, WFP_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    json_t * response = json_object();
    json_object_set_new(response, "result", json_string("okay"));
    json_object_set_new(response, "id", json_integer(42));

    wfp_jsonrpc_proxy_onresult(proxy, response);
    json_decref(response);

    wfp_jsonrpc_proxy_dispose(proxy);
    wfp_timer_manager_dispose(timer_manager);
}

