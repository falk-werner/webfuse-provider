#ifndef FAKE_INVOCATION_CONTEXT_HPP
#define FAKE_INVOCATION_CONTEXT_HPP

#include "webfuse_provider/impl/provider.h"
#include "webfuse_provider/mocks/mock_provider.hpp"

namespace webfuse_test
{

wfp_impl_invokation_context create_context(
    MockProvider& provider,
    wfp_request * request = nullptr);

}

#endif