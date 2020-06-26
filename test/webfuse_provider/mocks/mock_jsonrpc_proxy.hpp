#ifndef MOCK_JSONRPC_PROXY_HPP
#define MOCK_JSONRPC_PROXY_HPP

#include "webfuse_provider/impl/jsonrpc/proxy_intern.h"
#include <gmock/gmock.h>

namespace webfuse_test
{

class MockJsonRpcProxy
{
public:
    MockJsonRpcProxy();
    virtual ~MockJsonRpcProxy();
    MOCK_METHOD5(wfp_jsonrpc_proxy_vinvoke, void (
        struct wfp_jsonrpc_proxy * proxy,
        wfp_jsonrpc_proxy_finished_fn * finished,
        void * user_data,
        char const * method_name,
        char const * param_info));
    MOCK_METHOD3(wfp_jsonrpc_proxy_vnotify, void (
        struct wfp_jsonrpc_proxy * proxy,
        char const * method_name,
        char const * param_info));

};

}

#endif
