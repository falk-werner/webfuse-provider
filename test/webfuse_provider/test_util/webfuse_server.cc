#include "webfuse_provider/test_util/webfuse_server.hpp"
#include "webfuse_provider/impl/util/lws_log.h"
#include "webfuse_provider/protocol_names.h"

#include <libwebsockets.h>
#include <stdexcept>
#include <thread>
#include <future>
#include <mutex>
#include <sstream>
#include <queue>
#include <chrono>

#define TIMEOUT (std::chrono::seconds(10))

namespace
{

class IServer
{
public:
    virtual ~IServer() = default;
    virtual void OnConnected(lws * wsi) = 0;
    virtual void OnConnectionClosed(lws * wsi) = 0;
    virtual void OnMessageReceived(lws * wsi, char const * data, size_t length) = 0;
    virtual void OnWritable(lws * wsi) = 0;
};

}

extern "C"
{

static int wfp_test_utils_webfuse_server_callback(
    lws * wsi,
    lws_callback_reasons reason,
    void * user,
    void * in,
    size_t len)
{
    int result = 0;
    lws_protocols const * protocol = lws_get_protocol(wsi);
    auto * server = reinterpret_cast<IServer*>(nullptr != protocol ? protocol->user : nullptr);

    if (nullptr != server)
    {
        switch (reason)
        {
            case LWS_CALLBACK_ESTABLISHED:
                server->OnConnected(wsi);
                break;
            case LWS_CALLBACK_CLOSED:
                server->OnConnectionClosed(wsi);
                break;
            case LWS_CALLBACK_RECEIVE:
                {
                    auto * data = reinterpret_cast<char const*>(in);
                    server->OnMessageReceived(wsi, data, len);
                }
                break;
            case LWS_CALLBACK_SERVER_WRITEABLE:
                server->OnWritable(wsi);
                break;
            default:
                break;
        }
    }

    return result;
}

}

namespace webfuse_test
{

class WebfuseServer::Private: public IServer
{
public:
    Private(bool use_tls)
    : id(0)
    , is_shutdown_requested(false)
    , message(nullptr)
    , client(nullptr)
    {
        wfp_impl_lwslog_disable();

        IServer * server = this;
        memset(protocols, 0, sizeof(struct lws_protocols) * 2 );

        protocols[0].name = WFP_PROTOCOL_NAME_ADAPTER_SERVER;
        protocols[0].callback = &wfp_test_utils_webfuse_server_callback;
        protocols[0].per_session_data_size = 0;
        protocols[0].user = reinterpret_cast<void*>(server);

        memset(&info, 0, sizeof(struct lws_context_creation_info));
        info.port = 0;
        info.mounts = NULL;
        info.protocols = protocols;
        info.vhost_name = "localhost";
        info.ws_ping_pong_interval = 10;
        info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
        info.options |= LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

        context = lws_create_context(&info);

        if (use_tls)
        {
            info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
            info.ssl_cert_filepath = "server-cert.pem";
            info.ssl_private_key_filepath = "server-key.pem";
        }

        struct lws_vhost * vhost = lws_create_vhost(context, &info);
        int port = lws_get_vhost_port(vhost);
        std::ostringstream stream;        
        stream << (use_tls ? "wss://" : "ws://") << "localhost:" << port << "/";
        url = stream.str();

        thread = std::thread(&Run, this);

    }

    ~Private() override
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            is_shutdown_requested = true;
        }

        lws_cancel_service(context);
        thread.join();
        lws_context_destroy(context);
    }

    std::string const & GetUrl() const
    {
        return url;
    }

    std::string const & GetFilesystem() const
    {
        return filesystem;
    }

    json_t * Invoke(std::string const & method, json_t * params)
    {
        std::promise<std::string> response;
        {            
            std::unique_lock<std::mutex> lock(mutex);
            message = &response;
            id++;

            json_t * request = json_object();
            json_object_set_new(request, "method", json_string(method.c_str()));
            json_object_set_new(request, "params", params);
            json_object_set_new(request, "id", json_integer(id));

            char * request_text = json_dumps(request, 0);
            write_queue.push(request_text);
            free(request_text);
            json_decref(request);
        }
        lws_callback_on_writable(client);

        json_t * result = nullptr;
        auto future = response.get_future();
        auto state = future.wait_for(TIMEOUT);
        if (std::future_status::ready == state)
        {
            std::string response_text = future.get();
            result = json_loadb(response_text.c_str(), response_text.size(), 0, nullptr);
            std::unique_lock<std::mutex> lock(mutex);
            message = nullptr;
        }

        return result;
    }

    void OnConnected(lws * wsi) override
    {
        std::unique_lock<std::mutex> lock(mutex);
        client = wsi;
    }

    void OnConnectionClosed(lws * wsi) override
    {
        std::unique_lock<std::mutex> lock(mutex);
        client = nullptr;
    }

    void OnMessageReceived(lws * wsi, char const * data, size_t length) override
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (nullptr != message)
            {
                message->set_value(std::string(data, length));
            }
        }

        json_t * message = json_loadb(data, length, 0, nullptr);
        if (message)
        {
            json_t * method = json_object_get(message, "method");
            if (json_is_string(method))
            {
                if (0 == strcmp("add_filesystem", json_string_value(method)))
                {
                    json_t * id = json_object_get(message, "id");

                    json_t * response = json_object();
                    json_t * result = json_object();
                    json_object_set_new(result, "id", json_string(GetFilesystem().c_str())); 
                    json_object_set_new(response, "result", result);
                    json_object_set(response, "id", id);

                    char * response_text = json_dumps(response, 0);
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        write_queue.push(response_text);
                    }
                    free(response_text);
                    json_decref(response);

                    lws_callback_on_writable(wsi);
                }
            }
            
            json_decref(message);
        }
    }

    void OnWritable(lws * wsi) override
    {
        bool notify = false;

        {
            std::unique_lock<std::mutex> lock(mutex);

            if (!write_queue.empty())
            {
                std::string const & message = write_queue.front();

                unsigned char * data = new unsigned char[LWS_PRE + message.size()];
                memcpy(&data[LWS_PRE], message.c_str(), message.size());
                lws_write(wsi, &data[LWS_PRE], message.size(), LWS_WRITE_TEXT);
                delete[] data;

                write_queue.pop();
                notify = !write_queue.empty();
            }
        }

        if (notify)
        {
            lws_callback_on_writable(wsi);
        }
    }



private:
    static void Run(Private * self)
    {
        bool is_running = true;
        while (is_running)
        {
            lws_service(self->context, 0);
            {
                std::unique_lock<std::mutex> lock(self->mutex);
                is_running = !self->is_shutdown_requested;
            }
        }
    }

    int id;
    bool is_shutdown_requested;
    std::promise<std::string> * message;
    lws * client;
    std::string url;
    lws_context * context;
    lws_protocols protocols[2];
    lws_context_creation_info info;
    std::thread thread;
    std::mutex mutex;
    std::queue<std::string> write_queue;

    std::string filesystem = "fs";
};


WebfuseServer::WebfuseServer(bool use_tls)
: d(new Private(use_tls))
{

}

WebfuseServer::~WebfuseServer()
{
    delete d;
}

std::string const & WebfuseServer::GetUrl()
{
    return d->GetUrl();
}

json_t * WebfuseServer::Invoke(std::string const & method, json_t * params)
{
    return d->Invoke(method, params);
}

json_t * WebfuseServer::Invoke(std::string const & method, std::string const & params)
{
    json_t * params_json = json_loads(params.c_str(), 0, nullptr);
    return d->Invoke(method, params_json);
}

json_t * WebfuseServer::Lookup(int parent, std::string const & name)
{
    json_t * params = json_array();
    json_array_append_new(params, json_string(d->GetFilesystem().c_str()));
    json_array_append_new(params, json_integer(parent));
    json_array_append_new(params, json_string(name.c_str()));

    return d->Invoke("lookup", params);
}

json_t * WebfuseServer::Open(int inode, int flags)
{
    json_t * params = json_array();
    json_array_append_new(params, json_string(d->GetFilesystem().c_str()));
    json_array_append_new(params, json_integer(inode));
    json_array_append_new(params, json_integer(flags));

    return d->Invoke("open", params);
}

json_t * WebfuseServer::Read(int inode, int handle, int offset, int length)
{
    json_t * params = json_array();
    json_array_append_new(params, json_string(d->GetFilesystem().c_str()));
    json_array_append_new(params, json_integer(inode));
    json_array_append_new(params, json_integer(handle));
    json_array_append_new(params, json_integer(offset));
    json_array_append_new(params, json_integer(length));

    return d->Invoke("read", params);
}

json_t * WebfuseServer::ReadDir(int inode)
{
    json_t * params = json_array();
    json_array_append_new(params, json_string(d->GetFilesystem().c_str()));
    json_array_append_new(params, json_integer(inode));

    return d->Invoke("readdir", params);
}

}