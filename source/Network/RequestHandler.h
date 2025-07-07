#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include "../System/HtaccessConfig.h"

namespace Network {

class RequestHandler {
public:
    // Constructor
    RequestHandler(boost::asio::io_context& ioContext, const std::string& rootDir);

    // Handle incoming HTTP request
    void handleRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

private:
    // Serve static file content
    void serveStaticFile(const std::string& path, std::stringstream& responseStream);

    // Handle PHP script execution
    void handlePhpRequest(const std::string& path, std::stringstream& requestStream, std::stringstream& responseStream);

    boost::asio::io_context& ioContext_; // Reference to io_context for async operations
    std::string rootDir_; // Root directory for serving files
    System::HtaccessConfig htaccessConfig_; // Configuration from .htaccess
};

} // namespace Network

#endif // REQUESTHANDLER_H