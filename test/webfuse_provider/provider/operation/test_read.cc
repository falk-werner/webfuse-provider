#include "webfuse_provider/impl/operation/read.h"
#include "webfuse_provider/mocks/mock_request.hpp"
#include "webfuse_provider/mocks/mock_provider.hpp"
#include "webfuse_provider/mocks/fake_invokation_context.hpp"
#include "webfuse_provider/test_util/json_doc.hpp"

#include <gtest/gtest.h>
#include <cstdlib>

using ::webfuse_test::MockProvider;
using ::webfuse_test::MockRequest;
using ::webfuse_test::ReadResultMatcher;
using ::webfuse_test::create_context;
using ::webfuse_test::JsonDoc;
using ::testing::_;
using ::testing::Invoke;
using ::testing::StrEq;

namespace
{

void free_request(wfp_request * request, ino_t, uint32_t, size_t ,size_t)
{
    wfp_impl_request_dispose(request);
}

}

TEST(wfp_impl_read, invoke_provider)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, 42, 5, 2, 1)).Times(1)
        .WillOnce(Invoke(free_request));

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\",42,5,2,1]");
    wfp_impl_read(&context, doc.root(), 42);
}

TEST(wfp_impl_read, fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesyste\",1,2,3,4,5]");
    wfp_impl_read(&context, doc.root(), 42);
}

TEST(wfp_impl_read, fail_invalid_inode_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesyste\",\"42\",2,3,4]");
    wfp_impl_read(&context, doc.root(), 42);
}

TEST(wfp_impl_read, fail_invalid_handle_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesyste\",1,\"42\",3,4]");
    wfp_impl_read(&context, doc.root(), 42);
}

TEST(wfp_impl_read, fail_invalid_offset_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesyste\",1,2,\"42\",4]");
    wfp_impl_read(&context, doc.root(), 42);
}

TEST(wfp_impl_read, fail_invalid_length_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesyste\",1,2,3,\"42\"]");
    wfp_impl_read(&context, doc.root(), 42);
}

TEST(wfp_impl_read, default_responds_error)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond_error(_,42)).Times(1);

    wfp_impl_read_default(req, 0, 0, 1, 2, nullptr);
}

TEST(wfp_impl_read, respond)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(ReadResultMatcher("d2Y=", "base64", 2), 42)).Times(1);

    char const data[] = "wf";
    wfp_impl_respond_read(req, data, 2);
}

TEST(wfp_impl_read, respond_empty)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(ReadResultMatcher("", "identity", 0), 42)).Times(1);

    wfp_impl_respond_read(req, nullptr, 0);
}
