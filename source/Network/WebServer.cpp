//
// Created by doniff on 10/31/2024.
//

#include "WebServer.h"

namespace Network {

    WebServer::WebServer(boost::asio::io_context& ioContext, const std::vector<std::pair<int, std::string>>& projects)
        : ioContext_(ioContext) {
        for (const auto& project : projects) {
            startAccept(project.first, project.second);
        }
    }

    void WebServer::start() const
    {
        ioContext_.run();
    }

    void WebServer::startAccept(const int port, const std::string& rootDir)
    {
        auto acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(ioContext_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
        auto handler = std::make_shared<RequestHandler>(ioContext_, rootDir);

        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioContext_);
        acceptor->async_accept(*socket, [this, acceptor = acceptor.get(), handler, socket](const boost::system::error_code& error) {
            if (!error) {
                std::thread(&RequestHandler::handleRequest, handler, socket).detach();
            }
            startAccept(acceptor->local_endpoint().port(), handler->getRootDirectory());
        });

        acceptors_.push_back(std::move(acceptor));
        handlers_.push_back(handler);
    }
} // Network