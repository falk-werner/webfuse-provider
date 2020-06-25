#ifndef WFP_TEST_UTIL_CLIENT_HPP
#define WFP_TEST_UTIL_CLIENT_HPP

#include "webfuse_provider/client_config.h"
#include <string>

namespace webfuse_test
{

class Client
{
    Client(Client const &) = delete;
    Client & operator=(Client const &) = delete;
public:
    Client(wfp_client_config * config, std::string const & url);
    ~Client();
    void Disconnect();
private:
    class Private;
    Private * d;
};

}

#endif
