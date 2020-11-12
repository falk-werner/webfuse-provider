#ifndef WFP_TEST_UTILS_WS_SERVER_HPP
#define WFP_TEST_UTILS_WS_SERVER_HPP

#include <string>
#include <functional>

namespace webfuse_test
{

class WsServer
{
    WsServer(WsServer const &) = delete;
    WsServer & operator=(WsServer const &) = delete;
public:
    WsServer(
        std::string const & protocol,
        std::function<void(std::string const &)> handleMessage,
        int port = 0);
    ~WsServer();
    bool IsConnected();
    std::string GetUrl() const;
    void SendMessage(std::string const & message);
private:
    class Private;
    Private * d;
};

}


#endif
