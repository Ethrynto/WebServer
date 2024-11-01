#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <boost/asio.hpp>
#include <memory>
#include <string>

namespace Network {

class RequestHandler {
private:
    void handlePhpRequest(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::string& filePath);
    static void sendNotFoundRequest(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
    static std::string getContentType(const std::string& filePath);

    boost::asio::io_context& ioContext_;
    std::string rootDir_;
public:
    RequestHandler(boost::asio::io_context& ioContext, std::string  rootDir);
    void handleRequest(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
    std::string getRootDirectory() const;
};

} // Network

#endif //REQUESTHANDLER_H
