#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <webfuse_provider/client_protocol.h>
#include <webfuse_provider/client_config.h>
#include "webfuse_provider/test_util/ws_server.h"
#include "webfuse_provider/mocks/mock_provider_client.hpp"
#include "webfuse_provider/protocol_names.h"
#include "webfuse_provider/test_util/timeout_watcher.hpp"
#include "webfuse_provider/impl/json/parser.h"
#include "webfuse_provider/impl/json/node.h"

#include <libwebsockets.h>

#include <cstring>
#include <sstream>
#include <thread>

using webfuse_test::WsServer;
using webfuse_test::MockProviderClient;
using webfuse_test::IProviderClient;
using webfuse_test::TimeoutWatcher;
using testing::_;
using testing::AtMost;
using testing::Invoke;

#define DEFAULT_TIMEOUT (std::chrono::milliseconds(5 * 1000))

namespace
{

class ClientProtocolFixture
{
    ClientProtocolFixture(ClientProtocolFixture const &) = delete;
    ClientProtocolFixture& operator=(ClientProtocolFixture const &) = delete;
public:
    explicit ClientProtocolFixture(IProviderClient& client, bool enableAuthentication = false)
    {
        server = new WsServer(WFP_PROTOCOL_NAME_ADAPTER_SERVER);

        config = wfp_client_config_create();
        client.AttachTo(config, enableAuthentication);
        protocol = wfp_client_protocol_create(config);

        memset(protocols, 0, sizeof(struct lws_protocols) * 2);
        wfp_client_protocol_init_lws(protocol, protocols);

        memset(&info, 0, sizeof(struct lws_context_creation_info));
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.uid = -1;
        info.gid = -1;

        context = lws_create_context(&info);
    }

    ~ClientProtocolFixture()
    {
        lws_context_destroy(context);
        wfp_client_protocol_dispose(protocol);
        wfp_client_config_dispose(config);
        delete server;
    }

    void Connect()
    {
        TimeoutWatcher watcher(DEFAULT_TIMEOUT);

        wfp_client_protocol_connect(protocol, context, server->GetUrl().c_str());        
        while (!server->IsConnected())
        {
            watcher.check();
            lws_service(context, 0);
        }
    }

    void Disconnect()
    {
        wfp_client_protocol_disconnect(protocol);
    }

    void SendToClient(std::string const & request)
    {
        server->SendMessage(request);
    }

    std::string ReceiveMessageFromClient()
    {
        TimeoutWatcher watcher(DEFAULT_TIMEOUT);
        std::string result = server->ReceiveMessage();
        while (result.empty())
        {
            watcher.check();
            lws_service(context, 0);
            result = server->ReceiveMessage();
        }

        return result;
    }

    void AwaitAuthentication(
        std::string const & expected_username,
        std::string const & expected_password)
    {
        std::string request_text = ReceiveMessageFromClient();
        wfp_json_doc * doc = wfp_impl_json_parse(const_cast<char *>(request_text.data()));
        wfp_json const * request = wfp_impl_json_root(doc);
        ASSERT_TRUE(wfp_impl_json_is_object(request));

        wfp_json const * method = wfp_impl_json_object_get(request, "method");
        ASSERT_TRUE(wfp_impl_json_is_string(method));
        ASSERT_STREQ("authenticate", wfp_impl_json_get_string(method));

        wfp_json const * id = wfp_impl_json_object_get(request, "id");
        ASSERT_TRUE(wfp_impl_json_is_int(id));

        wfp_json const * params = wfp_impl_json_object_get(request, "params");
        ASSERT_TRUE(wfp_impl_json_is_array(params));
        ASSERT_EQ(2, wfp_impl_json_array_size(params));

        wfp_json const * type = wfp_impl_json_array_get(params, 0);
        ASSERT_TRUE(wfp_impl_json_is_string(type));
        ASSERT_STREQ("username", wfp_impl_json_get_string(type));

        wfp_json const * credentials = wfp_impl_json_array_get(params, 1);
        ASSERT_TRUE(wfp_impl_json_is_object(credentials));

        wfp_json const * username = wfp_impl_json_object_get(credentials, "username");
        ASSERT_TRUE(wfp_impl_json_is_string(username));
        ASSERT_STREQ(expected_username.c_str(), wfp_impl_json_get_string(username));
        
        wfp_json const * password = wfp_impl_json_object_get(credentials, "password");
        ASSERT_TRUE(wfp_impl_json_is_string(password));
        ASSERT_STREQ(expected_password.c_str(), wfp_impl_json_get_string(password));

        std::ostringstream response;
        response << "{\"result\": {}, \"id\": " << wfp_impl_json_get_int(id) << "}";
        SendToClient(response.str());

        wfp_impl_json_dispose(doc);
    }

    void AwaitAddFilesystem(std::string& filesystemName)
    {
        std::string request_text = ReceiveMessageFromClient();
        wfp_json_doc * doc = wfp_impl_json_parse(const_cast<char *>(request_text.data()));
        wfp_json const * request = wfp_impl_json_root(doc);
        ASSERT_TRUE(wfp_impl_json_is_object(request));

        wfp_json const * method = wfp_impl_json_object_get(request, "method");
        ASSERT_TRUE(wfp_impl_json_is_string(method));
        ASSERT_STREQ("add_filesystem", wfp_impl_json_get_string(method));

        wfp_json const * params = wfp_impl_json_object_get(request, "params");
        ASSERT_TRUE(wfp_impl_json_is_array(params));
        ASSERT_EQ(1, wfp_impl_json_array_size(params));

        wfp_json const * filesystem = wfp_impl_json_array_get(params, 0);
        ASSERT_TRUE(wfp_impl_json_is_string(filesystem));

        wfp_json const * id = wfp_impl_json_object_get(request, "id");
        ASSERT_TRUE(wfp_impl_json_is_int(id));

        std::ostringstream response;
        response << "{\"result\": {\"id\": \"" << wfp_impl_json_get_string(filesystem) << "\"}, \"id\": " << wfp_impl_json_get_int(id) << "}";

        SendToClient(response.str());

        wfp_impl_json_dispose(doc);
    }

private:
    WsServer * server;
    wfp_client_config * config;
    wfp_client_protocol * protocol;
    struct lws_context_creation_info info;
    struct lws_protocols protocols[2];
    struct lws_context * context;

};

void GetCredentials(wfp_credentials * credentials)
{
    wfp_credentials_set_type(credentials, "username");
    wfp_credentials_add(credentials, "username", "bob");
    wfp_credentials_add(credentials, "password", "secret");
}

}

TEST(client_protocol, connect)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider);

    EXPECT_CALL(provider, OnConnected()).Times(AtMost(1));
    EXPECT_CALL(provider, OnDisconnected()).Times(1);

    fixture.Connect();
    if (HasFatalFailure()) { return; }

    std::string filesystem;
    fixture.AwaitAddFilesystem(filesystem);
    if (HasFatalFailure()) { return; }
}

TEST(client_protocol, disconnect_without_connect)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider);

    EXPECT_CALL(provider, OnDisconnected()).Times(1);

    fixture.Disconnect();
}

TEST(client_protocol, connect_with_username_authentication)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider, true);

    EXPECT_CALL(provider, OnConnected()).Times(AtMost(1));
    EXPECT_CALL(provider, OnDisconnected()).Times(1);
    EXPECT_CALL(provider, GetCredentials(_)).Times(1).WillOnce(Invoke(GetCredentials));
    
    fixture.Connect();
    if (HasFatalFailure()) { return; }

    fixture.AwaitAuthentication("bob", "secret");
    if (HasFatalFailure()) { return; }

    std::string filesystem;
    fixture.AwaitAddFilesystem(filesystem);
    if (HasFatalFailure()) { return; }
}

TEST(client_protocol, getattr)
{
    MockProviderClient provider;
    ClientProtocolFixture fixture(provider);

    EXPECT_CALL(provider, OnConnected()).Times(1);
    EXPECT_CALL(provider, OnDisconnected()).Times(1);
    EXPECT_CALL(provider, GetAttr(1, _)).Times(1);

    fixture.Connect();
    if (HasFatalFailure()) { return; }

    std::string filesystem;
    fixture.AwaitAddFilesystem(filesystem);
    if (HasFatalFailure()) { return; }

    std::ostringstream request;
    request << "{\"method\": \"getattr\", \"params\": [\"" << filesystem << "\", 1], \"id\": 42}";

    fixture.SendToClient(request.str());
    std::string response = fixture.ReceiveMessageFromClient();
    ASSERT_FALSE(response.empty());

    fixture.Disconnect();
}