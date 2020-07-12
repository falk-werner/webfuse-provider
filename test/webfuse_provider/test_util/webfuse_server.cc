#include "webfuse_provider/test_util/webfuse_server.hpp"
#include "webfuse_provider/impl/util/lws_log.h"
#include "webfuse_provider/protocol_names.h"
#include "webfuse_provider/impl/json/doc.h"
#include "webfuse_provider/impl/json/node.h"

#include <libwebsockets.h>
#include <stdexcept>
#include <thread>
#include <future>
#include <mutex>
#include <sstream>
#include <queue>
#include <chrono>
#include <sstream>

#define TIMEOUT (std::chrono::seconds(10))

namespace
{

class IServer
{
public:
    virtual ~IServer() = default;
    virtual void OnConnected(lws * wsi) = 0;
    virtual void OnConnectionClosed(lws * wsi) = 0;
    virtual void OnMessageReceived(lws * wsi, char * data, size_t length) = 0;
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
                    auto * data = reinterpret_cast<char*>(in);
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

    std::string Invoke(std::string const & method, std::string const & params)
    {
        std::promise<std::string> response;
        {            
            std::unique_lock<std::mutex> lock(mutex);
            message = &response;
            id++;

            std::ostringstream request;
            request << "{"
                << "\"method\": \"" << method << "\","
                << "\"params\": " << params << ","
                << "\"id\": " << id
                << "}";

            write_queue.push(request.str());
        }
        lws_callback_on_writable(client);

        std::string result;
        auto future = response.get_future();
        auto state = future.wait_for(TIMEOUT);
        if (std::future_status::ready == state)
        {
            result = future.get();
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

    void OnMessageReceived(lws * wsi, char * data, size_t length) override
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (nullptr != message)
            {
                message->set_value(std::string(data, length));
            }
        }

        wfp_json_doc * doc = wfp_impl_json_doc_loadb(data, length);
        if (doc)
        {
            wfp_json const * message = wfp_impl_json_doc_root(doc);
            wfp_json const * method = wfp_impl_json_object_get(message, "method");
            if (wfp_impl_json_is_string(method))
            {
                if (0 == strcmp("add_filesystem", wfp_impl_json_get_string(method)))
                {
                    wfp_json const * id = wfp_impl_json_object_get(message, "id");

                    std::ostringstream response;
                    response << "{\"result\": {\"id\": \"" << GetFilesystem() << "\"}, "
                        << "\"id\": " << wfp_impl_json_get_int(id) << "}";
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        write_queue.push(response.str());
                    }

                    lws_callback_on_writable(wsi);
                }
            }
            
            wfp_impl_json_doc_dispose(doc);
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

std::string WebfuseServer::Invoke(std::string const & method, std::string const & params)
{
    return d->Invoke(method, params);
}

std::string WebfuseServer::Lookup(int parent, std::string const & name)
{
    std::ostringstream params;
    params << "[\"" << d->GetFilesystem() << "\", " << parent << ", \"" << name << "\"]";

    return d->Invoke("lookup", params.str());
}

std::string WebfuseServer::Open(int inode, int flags)
{
    std::ostringstream params;
    params << "[\"" << d->GetFilesystem() << "\", " << inode << ", " << flags << "]";

    return d->Invoke("open", params.str());
}

std::string WebfuseServer::Read(int inode, int handle, int offset, int length)
{
    std::ostringstream params;
    params << "[\"" << d->GetFilesystem() << "\", " << inode << ", " << handle << ", " << offset << ", " << length << "]";

    return d->Invoke("read", params.str());
}

std::string WebfuseServer::ReadDir(int inode)
{
    std::ostringstream params;
    params << "[\"" << d->GetFilesystem() << "\", " << inode << "]";

    return d->Invoke("readdir", params.str());
}

}