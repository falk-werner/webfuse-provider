#include "webfuse_provider/impl/operation/getattr.h"
#include "webfuse_provider/mocks/mock_request.hpp"
#include "webfuse_provider/mocks/mock_provider.hpp"
#include "webfuse_provider/mocks/fake_invokation_context.hpp"
#include "webfuse_provider/test_util/json_doc.hpp"

#include <gtest/gtest.h>
#include <cstdlib>

using ::webfuse_test::MockProvider;
using ::webfuse_test::MockRequest;
using ::webfuse_test::StatMatcher;
using ::webfuse_test::create_context;
using ::webfuse_test::JsonDoc;
using ::testing::_;
using ::testing::Invoke;

namespace
{

void free_request(wfp_request * request, ino_t)
{
    wfp_impl_request_dispose(request);
}

}

TEST(wfp_impl_getattr, default_responds_error)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond_error(_,42)).Times(1);

    wfp_impl_getattr_default(req, 0, nullptr);
}

TEST(wfp_impl_getattr, respond_file)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(StatMatcher(23, 0754, "file"), 42)).Times(1);

    struct stat buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.st_ino = 23;
    buffer.st_mode = S_IFREG | 0754;
    wfp_impl_respond_getattr(req, &buffer);
}

TEST(wfp_impl_getattr, respond_dir)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(StatMatcher(23, 0754, "dir"), 42)).Times(1);

    struct stat buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.st_ino = 23;
    buffer.st_mode = S_IFDIR | 0754;
    wfp_impl_respond_getattr(req, &buffer);
}

TEST(wfp_impl_getattr, invoke_provider)
{
    MockProvider provider;
    EXPECT_CALL(provider,getattr(_, 23)).Times(1).WillOnce(Invoke(free_request));

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\", 23]");
    wfp_impl_getattr(&context, doc.root(), 42);
}

TEST(wfp_impl_getattr, fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider,getattr(_, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\"]");
    wfp_impl_getattr(&context, doc.root(), 42);
}

TEST(wfp_impl_getattr, fail_invalid_inode_type)
{
    MockProvider provider;
    EXPECT_CALL(provider,getattr(_, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    JsonDoc doc("[\"test.filesystem\", \"42\"]");
    wfp_impl_getattr(&context, doc.root(), 42);
}
