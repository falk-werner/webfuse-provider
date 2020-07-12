#ifndef WFP_TEST_UTIL_WEBFUSE_SERVER_HPP
#define WFP_TEST_UTIL_WEBFUSE_SERVER_HPP

#include <string>

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
    std::string Invoke(std::string const & method, std::string const & params);
    std::string Lookup(int parent, std::string const & name);
    std::string Open(int inode, int flags);
    std::string Read(int inode, int handle, int offset, int length);
    std::string ReadDir(int inode);
private:
    class Private;
    Private * d;
};

}

#endif
