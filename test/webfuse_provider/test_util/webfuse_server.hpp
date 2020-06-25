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
    json_t * Invoke(std::string const & method, json_t * params);
    json_t * Invoke(std::string const & method, std::string const & params);
    json_t * Lookup(int parent, std::string const & name);
    json_t * Open(int inode, int flags);
private:
    class Private;
    Private * d;
};

}

#endif
