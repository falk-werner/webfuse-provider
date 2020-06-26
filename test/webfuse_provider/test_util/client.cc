#include "webfuse_provider/test_util/client.hpp"
#include "webfuse_provider/client.h"

#include <thread>
#include <mutex>
#include <queue>

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
        invoke(shutdown);

        thread.join();
        wfp_client_dispose(client);
    }

    void Disconnect()
    {
        invoke(disconnect);
    }

private:
    enum command
    {
        run,
        shutdown,
        disconnect
    };

    void invoke(command cmd)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            commands.push(cmd);
        }

        wfp_client_interrupt(client);
    }

    static void Run(Private * self)
    {
        bool is_running = true;
        while (is_running)
        {
            switch (self->get_command())
            {
                case run:
                    wfp_client_service(self->client);
                    break;
                case shutdown:
                    is_running = false;
                    break;
                case disconnect:
                    wfp_client_disconnect(self->client);
                    break;
                default:
                    break;
            }
        }
    }

    command get_command()
    {
        command result = run;

        std::unique_lock<std::mutex> lock(mutex);
        if (!commands.empty())
        {
            result = commands.front();
            commands.pop();
        }

        return result;
    }

    wfp_client * client;
    bool is_shutdown_requested;
    std::thread thread;
    std::mutex mutex;
    std::queue<command> commands;
};

Client::Client(wfp_client_config * config, std::string const & url)
: d(new Private(config, url))
{

}

Client::~Client()
{
    delete d;
}

void Client::Disconnect()
{
    d->Disconnect();
}


}