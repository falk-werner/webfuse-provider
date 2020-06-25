#ifndef WFP_TEST_UTIL_WEBFUSE_SERVER_HPP
#define WFP_TEST_UTIL_WEBFUSE_SERVER_HPP

#include <string>
#include <jansson.h>

namespace webfuse_test
{

class WebfuseServer
{
    WebfuseServer(WebfuseServer const &) = delete;
    WebfuseServer& operator=(WebfuseServer const &) = delete;
public:
    WebfuseServer(bool use_tls = false);
    ~WebfuseServer();
    std::string const & GetUrl();
    void AwaitConnection();
    json_t * Invoke(std::string method, json_t * params);
private:
    class Private;
    Private * d;
};

}

#endif
