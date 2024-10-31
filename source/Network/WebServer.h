#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <memory>
#include <vector>

#include "RequestHandler.h"

namespace Network {

class WebServer {

private:
    void startAccept(int port, const std::string& rootDir);
    boost::asio::io_context& ioContext_;
    std::vector<std::unique_ptr<boost::asio::ip::tcp::acceptor>> acceptors_;
    std::vector<std::shared_ptr<RequestHandler>> handlers_;

public:
    WebServer(boost::asio::io_context& ioContext, const std::vector<std::pair<int, std::string>>& projects);
    void start() const;


};

} // Network

#endif //WEBSERVER_H
