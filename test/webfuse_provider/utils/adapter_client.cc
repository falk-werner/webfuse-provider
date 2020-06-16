#include "webfuse/utils/adapter_client.hpp"
#include "webfuse/utils/tempdir.hpp"
#include <thread>
#include <mutex>

namespace
{

enum class Command
{
    run,
    shutdown,
    connect,
    disconnect,
    authenticate,
    add_filesystem
};

}

namespace webfuse_test
{

class AdapterClient::Private
{
public:
    Private(
        wfp_client_callback_fn * callback,
        void * user_data,
        std::string const & url)
    : client(wfp_client_create(callback, user_data))
    , url_(url)
    , command(Command::run)
    , tempdir("webfuse_adpter_client")
    {
        thread = std::thread(&Run, this);
    }

    ~Private()
    {
        ApplyCommand(Command::shutdown);
        thread.join();
        wfp_client_dispose(client);
    }

    void ApplyCommand(Command actual_command)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            command = actual_command;
        }

        wfp_client_interrupt(client);
    }

    std::string GetDir()
    {
        return tempdir.path();
    }

private:
    static void Run(Private * self)
    {
        bool is_running = true;
        while (is_running)
        {
            Command actual_command;
            {
                std::unique_lock<std::mutex> lock(self->mutex);
                actual_command = self->command;
                self->command = Command::run;
            }

            switch (actual_command)
            {
                case Command::run:
                    wfp_client_service(self->client);
                    break;
                case Command::connect:
                    wfp_client_connect(self->client, self->url_.c_str());
                    break;
                case Command::disconnect:
                    wfp_client_disconnect(self->client);
                    break;
                case Command::authenticate:
                    wfp_client_authenticate(self->client);
                    break;
                case Command::add_filesystem:
                    wfp_client_add_filesystem(self->client, self->tempdir.path(), "test");
                    break;
                case Command::shutdown:
                    // fall-through
                default:
                    is_running = false;
                    break;
            }

        }
    }     

    wfp_client * client;
    std::string url_;
    Command command;
    TempDir tempdir;
    std::thread thread;
    std::mutex mutex;
};

AdapterClient::AdapterClient(
    wfp_client_callback_fn * callback,
    void * user_data,
    std::string const & url)
: d(new Private(callback, user_data, url))
{

}

AdapterClient::~AdapterClient()
{
    delete d;
}

void AdapterClient::Connect()
{
    d->ApplyCommand(Command::connect);
}

void AdapterClient::Disconnect()
{
    d->ApplyCommand(Command::disconnect);
}

void AdapterClient::Authenticate()
{
    d->ApplyCommand(Command::authenticate);
}

void AdapterClient::AddFileSystem()
{
    d->ApplyCommand(Command::add_filesystem);
}

std::string AdapterClient::GetDir() const
{
    return d->GetDir();
}


}