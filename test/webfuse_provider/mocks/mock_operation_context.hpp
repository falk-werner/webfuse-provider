#ifndef MOCK_OPERATIONS_CONTEXT_HPP
#define MOCK_OPERATIONS_CONTEXT_HPP

#include "webfuse/adapter/impl/operation/context.h"
#include <gmock/gmock.h>

namespace webfuse_test
{

class MockOperationContext
{
public:
    MockOperationContext();
    virtual ~MockOperationContext();
    MOCK_METHOD1(wfp_impl_operation_context_get_proxy, wfp_jsonrpc_proxy * (
	    struct wfp_impl_operation_context * context));

};

}

#endif
