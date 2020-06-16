#include "webfuse_provider/tests/integration/server.hpp"
#include <thread>
#include <mutex>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include "webfuse_adapter.h"
#include "webfuse/adapter/impl/server.h"

#define WFP_PATH_MAX (100)

extern "C"
{

static void webfuse_test_server_cleanup_mountpoint(
    void * user_data)
{
    char * path = reinterpret_cast<char*>(user_data);
    rmdir(path);
    free(path);
}

static struct wfp_mountpoint *
webfuse_test_server_create_mountpoint(
    char const * filesystem,
    void * user_data)
{
    char const * base_dir = reinterpret_cast<char const*>(user_data);
    char path[WFP_PATH_MAX];
    snprintf(path, WFP_PATH_MAX, "%s/%s", base_dir, filesystem);
    mkdir(path, 0755);
    struct wfp_mountpoint * mountpoint = wfp_mountpoint_create(path);
    wfp_mountpoint_set_userdata(
        mountpoint,
        reinterpret_cast<void*>(strdup(path)),
        &webfuse_test_server_cleanup_mountpoint);

    return mountpoint;
}

}

namespace webfuse_test
{

class Server::Private
{
public:
    Private()
    : is_shutdown_requested(false)
    {
        snprintf(base_dir, WFP_PATH_MAX, "%s", "/tmp/webfuse_test_integration_XXXXXX");
        char const * result = mkdtemp(base_dir);
        if (NULL == result)
        {
            throw std::runtime_error("unable to create temp dir");
        }


        config = wfp_server_config_create();
        wfp_server_config_set_port(config, 0);
        wfp_server_config_set_mountpoint_factory(config, 
            &webfuse_test_server_create_mountpoint,
            reinterpret_cast<void*>(base_dir));
        wfp_server_config_set_keypath(config, "server-key.pem");
        wfp_server_config_set_certpath(config, "server-cert.pem");

        server = wfp_server_create(config);

        while (!wfp_impl_server_is_operational(server))
        {
            wfp_server_service(server);
        }

        thread = std::thread(Run, this);

    }

    ~Private()
    {
        RequestShutdown();
        thread.join();
        rmdir(base_dir);
        wfp_server_dispose(server);
        wfp_server_config_dispose(config);
    }

    bool IsShutdownRequested()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        return is_shutdown_requested;
    }

    std::string GetUrl(void) const
    {
        int const port = wfp_server_get_port(server);
        std::ostringstream stream;
        stream << "wss://localhost:" << port << "/";
        return stream.str();
    }

private:
    void RequestShutdown()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        is_shutdown_requested = true;
        wfp_server_interrupt(server);
    }

    static void Run(Server::Private * context)
    {
        while (!context->IsShutdownRequested())
        {
            wfp_server_service(context->server);
        }
    }


    std::mutex shutdown_lock;
    std::thread thread;
    bool is_shutdown_requested;


public:
    char base_dir[WFP_PATH_MAX];
    wfp_server_config * config;
    wfp_server * server;
};

Server::Server()
: d(new Server::Private())
{

}

Server::~Server()
{
    delete d;
}

char const * Server::GetBaseDir(void) const
{
    return d->base_dir;
}

std::string Server::GetUrl(void) const
{
    return d->GetUrl();
}


}