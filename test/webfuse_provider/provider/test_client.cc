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
using testing::_;
using testing::StrEq;

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

TEST(Client, ConnectFailWithInvalidUrl)
{
    MockProviderClient provider;

    EXPECT_CALL(provider, OnConnected()).Times(0);

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        Client client(config, "invalid_url");

        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

TEST(Client, ConnectFailNotReachable)
{
    MockProviderClient provider;

    EXPECT_CALL(provider, OnConnected()).Times(0);

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        Client client(config, "ws://localhost:4/");

        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

TEST(Client, Lookup)
{
    MockProviderClient provider;

    std::promise<void> connected;
    EXPECT_CALL(provider, OnConnected()).Times(1)
        .WillOnce(Invoke([&]() { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    EXPECT_CALL(provider, Lookup(1,StrEq("foo"),_)).Times(1)
        .WillOnce(Invoke([](ino_t, char const *, struct stat * result) {
            result->st_ino = 42;
            result->st_mode = S_IFREG | 0644;
        }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        WebfuseServer server;
        Client client(config, server.GetUrl());

        ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

        json_t * response = server.Lookup(1, "foo");
        ASSERT_TRUE(json_is_object(response));
        json_t * result = json_object_get(response, "result");

        json_t * inode = json_object_get(result, "inode");
        ASSERT_EQ(42, json_integer_value(inode));

        json_t * mode = json_object_get(result, "mode");
        ASSERT_EQ(0644, json_integer_value(mode));

        json_t * type = json_object_get(result, "type");
        ASSERT_STREQ("file", json_string_value(type));

        json_decref(response);

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

TEST(Client, LookupFail)
{
    MockProviderClient provider;

    std::promise<void> connected;
    EXPECT_CALL(provider, OnConnected()).Times(1)
        .WillOnce(Invoke([&]() { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    EXPECT_CALL(provider, Lookup(1,StrEq("foo"),_)).Times(1)
        .WillOnce(Invoke([](ino_t, char const *, struct stat * result) {
            throw std::runtime_error("something went wrong");
        }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        WebfuseServer server;
        Client client(config, server.GetUrl());

        ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

        json_t * response = server.Lookup(1, "foo");
        ASSERT_TRUE(json_is_object(response));
        json_t * error = json_object_get(response, "error");

        json_t * code = json_object_get(error, "code");
        ASSERT_NE(0, json_integer_value(code));

        json_decref(response);

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

TEST(Client, Open)
{
    MockProviderClient provider;

    std::promise<void> connected;
    EXPECT_CALL(provider, OnConnected()).Times(1)
        .WillOnce(Invoke([&]() { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    EXPECT_CALL(provider, Open(1, 0, _)).Times(1)
        .WillOnce(Invoke([](int, int, uint32_t * handle) {
            *handle = 4711;
        }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        WebfuseServer server;
        Client client(config, server.GetUrl());

        ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

        json_t * response = server.Open(1, 0);
        ASSERT_TRUE(json_is_object(response));
        json_t * result = json_object_get(response, "result");

        json_t * handle = json_object_get(result, "handle");
        ASSERT_EQ(4711, json_integer_value(handle));

        json_decref(response);

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

