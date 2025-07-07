#include "WebServer.h"
#include "../Debug/Log.h"

namespace Network {

    WebServer::WebServer(boost::asio::io_context& ioContext, const std::vector<std::pair<int, std::string>>& projects)
            : ioContext_(ioContext), projects_(projects) {
        for (const auto& [port, path] : projects) {
            startAccept(port, path);
        }
    }

// Start the server
    void WebServer::start() {
        if (!running_) {
            running_ = true;
            // Start io_context in a separate thread
            ioThread_ = std::make_unique<std::thread>([this]() {
                try {
                    ioContext_.run();
                    Debug::Log::info("IO context stopped", "WebServer");
                } catch (const std::exception& e) {
                    Debug::Log::error(std::format("IO context error: {}", e.what()), "WebServer");
                }
            });
            Debug::Log::info(std::format("Server started with {} projects", projects_.size()), "WebServer");
        }
    }

// Stop the server
    void WebServer::stop() {
        if (running_) {
            running_ = false;
            for (auto& acceptor : acceptors_) {
                if (acceptor->is_open()) {
                    boost::system::error_code ec;
                    acceptor->close(ec);
                    if (ec) {
                        Debug::Log::error(std::format("Error closing acceptor: {}", ec.message()), "WebServer");
                    }
                }
            }
            ioContext_.stop();
            if (ioThread_ && ioThread_->joinable()) {
                ioThread_->join();
            }
            ioContext_.restart(); // Prepare io_context for next start
            acceptors_.clear();
            handlers_.clear();
            for (const auto& [port, path] : projects_) {
                startAccept(port, path); // Reinitialize acceptors for restart
            }
            Debug::Log::info("Server stopped", "WebServer");
        }
    }

// Start accepting connections on a specific port
    void WebServer::startAccept(int port, const std::string& rootDir) {
        try {
            auto acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(
                    ioContext_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
            auto handler = std::make_shared<RequestHandler>(ioContext_, rootDir);

            // Start asynchronous accept loop
            doAccept(acceptor.get(), handler, port, rootDir);

            acceptors_.push_back(std::move(acceptor));
            handlers_.push_back(std::move(handler));
            Debug::Log::info(std::format("Started acceptor on port {}", port), "WebServer");
        } catch (const std::exception& e) {
            Debug::Log::error(std::format("Failed to start acceptor on port {}: {}", port, e.what()), "WebServer");
        }
    }

// Handle asynchronous accept operations
    void WebServer::doAccept(boost::asio::ip::tcp::acceptor* acceptor, std::shared_ptr<RequestHandler> handler,
                             int port, const std::string& rootDir) {
        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(ioContext_);
        Debug::Log::info(std::format("Starting async_accept on port {}", port), "WebServer");
        acceptor->async_accept(*socket, [this, acceptor, handler, port, rootDir, socket](const boost::system::error_code& error) {
            if (!error) {
                Debug::Log::info(std::format("Accepted connection on port {}", port), "WebServer");
                // Process request in a separate thread
                std::thread(&RequestHandler::handleRequest, handler, socket).detach();
            } else if (running_) {
                Debug::Log::error(std::format("Accept error on port {}: {}", port, error.message()), "WebServer");
            }
            // Continue accepting connections if server is running
            if (running_) {
                Debug::Log::info(std::format("Preparing next async_accept on port {}", port), "WebServer");
                doAccept(acceptor, handler, port, rootDir);
            }
        });
    }

} // namespace Network