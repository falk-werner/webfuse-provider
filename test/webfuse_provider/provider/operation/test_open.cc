#include "webfuse_provider/impl/operation/open.h"
#include "webfuse_provider/mocks/mock_request.hpp"
#include "webfuse_provider/mocks/mock_provider.hpp"
#include "webfuse_provider/mocks/fake_invokation_context.hpp"
#include "webfuse_provider/test_util/json_doc.hpp"

#include <gtest/gtest.h>
#include <cstdlib>

using ::webfuse_test::MockProvider;
using ::webfuse_test::MockRequest;
using ::webfuse_test::OpenMatcher;
using ::webfuse_test::create_context;
using ::webfuse_test::JsonDoc;
using ::testing::_;
using ::testing::Invoke;
using ::testing::StrEq;

namespace
{

void free_request(wfp_request * request, ino_t, int)
{
    wfp_impl_request_dispose(request);
}

}

TEST(wfp_impl_open, invoke_provider)
{
    MockProvider provider;
    EXPECT_CALL(provider,open(_, 42, 0)).Times(1)
        .WillOnce(Invoke(free_request));

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\",42,0]");
    wfp_impl_open(&context, doc.root(), 42);
}

TEST(wfp_impl_open, fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider,open(_, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\", 23]");
    wfp_impl_open(&context, doc.root(), 42);
}

TEST(wfp_impl_open, fail_invalid_inode_type)
{
    MockProvider provider;
    EXPECT_CALL(provider,open(_, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\", \"\", 0]");
    wfp_impl_open(&context, doc.root(), 42);
}

TEST(wfp_impl_open, fail_invalid_flags_type)
{
    MockProvider provider;
    EXPECT_CALL(provider,open(_, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\", 23, \"\"]");
    wfp_impl_open(&context, doc.root(), 42);
}

TEST(wfp_impl_open, default_responds_error)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond_error(_,42)).Times(1);

    wfp_impl_open_default(req, 1, 0, nullptr);
}

TEST(wfp_impl_open, respond)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(OpenMatcher(23), 42)).Times(1);

    wfp_impl_respond_open(req, 23);
}

