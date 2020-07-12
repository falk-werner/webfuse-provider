#include "webfuse_provider/webfuse_provider.h"
#include "webfuse_provider/test_util/webfuse_server.hpp"
#include "webfuse_provider/mocks/mock_provider_client.hpp"
#include "webfuse_provider/test_util/client.hpp"
#include "webfuse_provider/test_util/json_doc.hpp"
#include "webfuse_provider/impl/json/node.h"

#include <gtest/gtest.h>
#include <future>
#include <chrono>

using webfuse_test::WebfuseServer;
using webfuse_test::MockProviderClient;
using webfuse_test::Client;
using webfuse_test::JsonDoc;
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

        JsonDoc doc(server.Lookup(1, "foo"));
        wfp_json const * response = doc.root();
        ASSERT_TRUE(wfp_impl_json_is_object(response));
        wfp_json const * result = wfp_impl_json_object_get(response, "result");

        wfp_json const * inode = wfp_impl_json_object_get(result, "inode");
        ASSERT_EQ(42, wfp_impl_json_get_int(inode));

        wfp_json const * mode = wfp_impl_json_object_get(result, "mode");
        ASSERT_EQ(0644, wfp_impl_json_get_int(mode));

        wfp_json const * type = wfp_impl_json_object_get(result, "type");
        ASSERT_STREQ("file", wfp_impl_json_get_string(type));

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

        JsonDoc doc(server.Lookup(1, "foo"));
        wfp_json const * response = doc.root();

        ASSERT_TRUE(wfp_impl_json_is_object(response));
        wfp_json const * error = wfp_impl_json_object_get(response, "error");

        wfp_json const * code = wfp_impl_json_object_get(error, "code");
        ASSERT_NE(0, wfp_impl_json_get_int(code));

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

        JsonDoc doc(server.Open(1, 0));
        wfp_json const * response = doc.root();

        ASSERT_TRUE(wfp_impl_json_is_object(response));
        wfp_json const * result = wfp_impl_json_object_get(response, "result");

        wfp_json const * handle = wfp_impl_json_object_get(result, "handle");
        ASSERT_EQ(4711, wfp_impl_json_get_int(handle));

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

TEST(Client, Read)
{
    MockProviderClient provider;

    std::promise<void> connected;
    EXPECT_CALL(provider, OnConnected()).Times(1)
        .WillOnce(Invoke([&]() { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    EXPECT_CALL(provider, Read(42, 5, 0, 1, _, _)).Times(1)
        .WillOnce(Invoke([](ino_t, uint32_t, size_t, size_t , char * buffer, size_t * bytes_read) {
            buffer[0] = 42;
            *bytes_read = 1;
        }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        WebfuseServer server;
        Client client(config, server.GetUrl());

        ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

        JsonDoc doc(server.Read(42, 5, 0, 1));
        wfp_json const * response = doc.root();

        ASSERT_TRUE(wfp_impl_json_is_object(response));
        wfp_json const * result = wfp_impl_json_object_get(response, "result");

        wfp_json const * format = wfp_impl_json_object_get(result, "format");
        ASSERT_STREQ("base64", wfp_impl_json_get_string(format));

        wfp_json const * count = wfp_impl_json_object_get(result, "count");
        ASSERT_EQ(1, wfp_impl_json_get_int(count));

        wfp_json const * data = wfp_impl_json_object_get(result, "data");
        ASSERT_STREQ("Kg==", wfp_impl_json_get_string(data));

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}

TEST(Client, ReadDir)
{
    MockProviderClient provider;

    std::promise<void> connected;
    EXPECT_CALL(provider, OnConnected()).Times(1)
        .WillOnce(Invoke([&]() { connected.set_value(); }));

    std::promise<void> disconnected;
    EXPECT_CALL(provider, OnDisconnected()).Times(1)
        .WillOnce(Invoke([&]() { disconnected.set_value(); }));

    EXPECT_CALL(provider, ReadDir(42, _)).Times(1)
        .WillOnce(Invoke([](ino_t, wfp_dirbuffer * buffer) {
            wfp_dirbuffer_add(buffer, ".", 42);
            wfp_dirbuffer_add(buffer, "..", 1);
            wfp_dirbuffer_add(buffer, "foo.txt", 43);
        }));

    wfp_client_config * config = wfp_client_config_create();
    provider.AttachTo(config);

    {
        WebfuseServer server;
        Client client(config, server.GetUrl());

        ASSERT_EQ(std::future_status::ready, connected.get_future().wait_for(TIMEOUT));

        JsonDoc doc(server.ReadDir(42));
        wfp_json const * response = doc.root();

        ASSERT_TRUE(wfp_impl_json_is_object(response));
        wfp_json const * result = wfp_impl_json_object_get(response, "result");

        ASSERT_TRUE(wfp_impl_json_is_array(result));
        ASSERT_EQ(3, wfp_impl_json_array_size(result));

        client.Disconnect();
        ASSERT_EQ(std::future_status::ready, disconnected.get_future().wait_for(TIMEOUT));
    }

    wfp_client_config_dispose(config);
}
