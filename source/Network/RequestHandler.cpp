#include "RequestHandler.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>

#include "../Debug/Log.h"

namespace Network {

    RequestHandler::RequestHandler(boost::asio::io_context& ioContext, std::string  rootDir)
    : ioContext_(ioContext), rootDir_(std::move(rootDir)) {}

void RequestHandler::handleRequest(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) {
    try {
        boost::asio::streambuf requestBuffer;
        boost::asio::read_until(*socket, requestBuffer, "\r\n\r\n");

        std::istream requestStream(&requestBuffer);
        std::string requestLine;
        std::getline(requestStream, requestLine);

        std::istringstream requestLineStream(requestLine);
        std::string method, path, version;
        requestLineStream >> method >> path >> version;

        if (method == "GET") {
            std::string filePath = rootDir_ + path;
            if (filePath.back() == '/')
                filePath += "index.html";

            if (filePath.ends_with(".php")) {
                handlePhpRequest(socket, filePath);
            } else {
                std::ifstream file(filePath, std::ios::binary);
                if (file) {
                    std::ostringstream responseStream;
                    responseStream << "HTTP/1.1 200 OK\r\n";
                    responseStream << "Content-Type: " << getContentType(filePath) << "\r\n";
                    responseStream << "Connection: close\r\n\r\n";
                    responseStream << file.rdbuf();

                    boost::asio::write(*socket, boost::asio::buffer(responseStream.str()));
                } else {
                    sendNotFoundRequest(socket);
                }
            }
        } else {
            sendNotFoundRequest(socket);
        }
    } catch (const std::exception& e) {
        Debug::Log::alert("Error handling request", "RequestHandler::handleRequest");
    }
}

std::string RequestHandler::getRootDirectory() const {
    return rootDir_;
}

void RequestHandler::handlePhpRequest(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::string& filePath) {
    const std::string command = "php " + filePath;
    std::array<char, 128> buffer {};
    std::string result;

    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        sendNotFoundRequest(socket);
        return;
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: text/html\r\n";
    responseStream << "Connection: close\r\n\r\n";
    responseStream << result;

    boost::asio::write(*socket, boost::asio::buffer(responseStream.str()));
}

std::string RequestHandler::getContentType(const std::string& filePath) {
    if (filePath.ends_with(".html") || filePath.ends_with(".php"))
        return "text/html";

    return "application/octet-stream";
}

void RequestHandler::sendNotFoundRequest(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
    std::string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
    boost::asio::write(*socket, boost::asio::buffer(response));
}

} // Network