#include "webfuse_provider/webfuse_provider.h"
#include "webfuse_provider/test_util/webfuse_server.hpp"
#include "webfuse_provider/mocks/mock_provider_client.hpp"
#include "webfuse_provider/test_util/client.hpp"

#include <gtest/gtest.h>
#include <future>
#include <chrono>

using webfuse_test::WebfuseServer;
using webfuse_test::MockProviderClient;
using webfuse_test::Client;
using testing::Invoke;

#define TIMEOUT (std::chrono::seconds(10))

TEST(Client, Connect)
{
    MockProviderClient provider;

    std::promise<void> connected;
    EXPECT_CALL(provider, OnConnected()).Times(1)
        .WillOnce(Invoke([&]() { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        WebfuseServer server;
        Client client(config, server.GetUrl());

        ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

TEST(Client, ConnectWithTls)
{
    MockProviderClient provider;

    std::promise<void> connected;
    EXPECT_CALL(provider, OnConnected()).Times(1)
        .WillOnce(Invoke([&]() { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    wfp_client_config * config = wfp_client_config_create();
    wfp_client_config_set_certpath(config, "client-cert.pem");
    wfp_client_config_set_keypath(config, "client-key.pem");
    wfp_client_config_set_ca_filepath(config, "server-cert.pem");
    provider.AttachTo(config);

    {
        WebfuseServer server(true);
        Client client(config, server.GetUrl());

        ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}