#include "webfuse_provider/test_util/client.hpp"
#include "webfuse_provider/client.h"

#include <thread>
#include <mutex>

namespace webfuse_test
{

class Client::Private
{
public:
    Private(wfp_client_config * config, std::string const & url)
    : client(wfp_client_create(config))
    , is_shutdown_requested(false)
    {
        wfp_client_connect(client, url.c_str());
        thread = std::thread(&Run, this);
    }

    ~Private()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            is_shutdown_requested = true;
        }

        wfp_client_interrupt(client);
        thread.join();
        wfp_client_disconnect(client);
        wfp_client_dispose(client);
    }

private:
    static void Run(Private * self)
    {
        bool is_running = true;
        while (is_running)
        {
            wfp_client_service(self->client);
            {
                std::unique_lock<std::mutex> lock(self->mutex);
                is_running = !self->is_shutdown_requested;
            }
        }
    }

    wfp_client * client;
    bool is_shutdown_requested;
    std::thread thread;
    std::mutex mutex;
};

Client::Client(wfp_client_config * config, std::string const & url)
: d(new Private(config, url))
{

}

Client::~Client()
{
    delete d;
}


}