#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <thread>
#include "RequestHandler.h"

namespace Network {

    class WebServer {
    public:
        WebServer(boost::asio::io_context& ioContext, const std::vector<std::pair<int, std::string>>& projects);

        // Start the server
        void start();

        // Stop the server
        void stop();

        // Check if server is running
        bool isRunning() const { return running_; }

        // Get list of projects
        const std::vector<std::pair<int, std::string>>& getProjects() const { return projects_; }

    private:
        // Start accepting connections on a specific port
        void startAccept(int port, const std::string& rootDir);

        // Handle asynchronous accept operations
        void doAccept(boost::asio::ip::tcp::acceptor* acceptor, std::shared_ptr<RequestHandler> handler,
                      int port, const std::string& rootDir);

        boost::asio::io_context& ioContext_;
        std::vector<std::unique_ptr<boost::asio::ip::tcp::acceptor>> acceptors_;
        std::vector<std::shared_ptr<RequestHandler>> handlers_;
        std::vector<std::pair<int, std::string>> projects_;
        bool running_ = false;
        std::unique_ptr<std::thread> ioThread_; // Thread for running io_context
    };

} // namespace Network

#endif // WEBSERVER_H