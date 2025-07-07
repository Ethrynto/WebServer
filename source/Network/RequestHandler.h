#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <boost/asio.hpp>
#include <string>
#include <memory>

namespace Network {

    class RequestHandler {
    public:
        explicit RequestHandler(boost::asio::io_context& ioContext, std::string rootDir);

        // Handle incoming HTTP request
        void handleRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

        // Get root directory of the project
        std::string getRootDirectory() const { return rootDir_; }

    private:
        // Handle PHP script execution
        void handlePhpRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const std::string& filePath);

        // Send 404 Not Found response
        void sendNotFound(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

        // Determine Content-Type based on file extension
        std::string getContentType(const std::string& filePath);

        boost::asio::io_context& ioContext_;
        std::string rootDir_;
    };

} // namespace Network

#endif // REQUESTHANDLER_H