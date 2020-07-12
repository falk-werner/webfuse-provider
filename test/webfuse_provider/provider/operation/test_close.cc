#include "webfuse_provider/impl/operation/close.h"
#include "webfuse_provider/mocks/mock_provider.hpp"
#include "webfuse_provider/mocks/fake_invokation_context.hpp"
#include "webfuse_provider/test_util/json_doc.hpp"

#include <gtest/gtest.h>

using ::webfuse_test::MockProvider;
using ::webfuse_test::create_context;
using ::webfuse_test::JsonDoc;
using ::testing::_;

TEST(wfp_close, close)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(42, 101, 23)).Times(1);

    wfp_impl_invokation_context context = create_context(provider);

    JsonDoc doc("[\"test.filesystem\", 42, 101, 23]");

    wfp_impl_close(&context, doc.root(), 42);
}

TEST(wfp_close, close_fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    JsonDoc doc("[]");

    wfp_impl_close(&context, doc.root(), 42);
}

TEST(wfp_close, close_fail_inode_invalid_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    JsonDoc doc("[\"test.filesystem\", \"42\", 0, 0]");

    wfp_impl_close(&context, doc.root(), 42);
}

TEST(wfp_close, close_fail_handle_invalid_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    JsonDoc doc("[\"test.filesystem\", 0, \"42\", 0]");

    wfp_impl_close(&context, doc.root(), 42);
}

TEST(wfp_close, close_fail_flags_invalid_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    JsonDoc doc("[\"test.filesystem\", 0, 0, \"42\"]");

    wfp_impl_close(&context, doc.root(), 42);
}


TEST(wfp_close, default_nop)
{
    wfp_impl_close_default(0, 0, 0, nullptr);
}