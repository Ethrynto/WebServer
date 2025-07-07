#include "RequestHandler.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../Debug/Log.h"

namespace Network {

    RequestHandler::RequestHandler(boost::asio::io_context& ioContext, std::string rootDir)
            : ioContext_(ioContext), rootDir_(std::move(rootDir)) {}

// Handle incoming HTTP request
    void RequestHandler::handleRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
        try {
            Debug::Log::info("Handling new request", "RequestHandler");
            boost::asio::streambuf requestBuffer;
            boost::asio::read_until(*socket, requestBuffer, "\r\n\r\n");

            std::istream requestStream(&requestBuffer);
            std::string requestLine;
            std::getline(requestStream, requestLine);

            std::istringstream requestLineStream(requestLine);
            std::string method, path, version;
            requestLineStream >> method >> path >> version;

            Debug::Log::info(std::format("Received {} request for {}", method, path), "RequestHandler");

            if (method != "GET") {
                Debug::Log::warn("Unsupported method: " + method, "RequestHandler");
                sendNotFound(socket);
                return;
            }

            std::string filePath = rootDir_ + path;
            if (filePath.ends_with("/")) filePath += "index.html";

            if (!std::filesystem::exists(filePath)) {
                Debug::Log::warn(std::format("File not found: {}", filePath), "RequestHandler");
                sendNotFound(socket);
                return;
            }

            if (filePath.ends_with(".php")) {
                handlePhpRequest(socket, filePath);
            } else {
                std::ifstream file(filePath, std::ios::binary);
                if (!file) {
                    Debug::Log::error(std::format("Failed to open file: {}", filePath), "RequestHandler");
                    sendNotFound(socket);
                    return;
                }
                std::ostringstream responseStream;
                responseStream << "HTTP/1.1 200 OK\r\n";
                responseStream << "Content-Type: " << getContentType(filePath) << "\r\n";
                responseStream << "Connection: close\r\n\r\n";
                responseStream << file.rdbuf();
                boost::asio::write(*socket, boost::asio::buffer(responseStream.str()));
                Debug::Log::info(std::format("Served static file: {}", filePath), "RequestHandler");
            }
        } catch (const std::exception& e) {
            Debug::Log::error(std::format("Request handling error: {}", e.what()), "RequestHandler");
            sendNotFound(socket);
        }
    }

// Handle PHP script execution
    void RequestHandler::handlePhpRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const std::string& filePath) {
        try {
            std::string command =
#ifdef _WIN32
                    "php \"" + filePath + "\"";
#else
            "php-cgi \"" + filePath + "\"";
#endif
            Debug::Log::info(std::format("Executing PHP: {}", command), "RequestHandler");
            std::array<char, 128> buffer{};
            std::string result;

            std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
            if (!pipe) {
                Debug::Log::error("Failed to execute PHP script", "RequestHandler");
                sendNotFound(socket);
                return;
            }

            while (fgets(buffer.data(), buffer.size(), pipe.get())) {
                result += buffer.data();
            }

            std::ostringstream responseStream;
            responseStream << "HTTP/1.1 200 OK\r\n";
            responseStream << "Content-Type: text/html\r\n";
            responseStream << "Connection: close\r\n\r\n";
            responseStream << result;
            boost::asio::write(*socket, boost::asio::buffer(responseStream.str()));
            Debug::Log::info(std::format("Served PHP file: {}", filePath), "RequestHandler");
        } catch (const std::exception& e) {
            Debug::Log::error(std::format("PHP execution error: {}", e.what()), "RequestHandler");
            sendNotFound(socket);
        }
    }

// Determine Content-Type based on file extension
    std::string RequestHandler::getContentType(const std::string& filePath) {
        if (filePath.ends_with(".html") || filePath.ends_with(".php")) return "text/html";
        if (filePath.ends_with(".css")) return "text/css";
        if (filePath.ends_with(".js")) return "application/javascript";
        return "application/octet-stream";
    }

// Send 404 Not Found response
    void RequestHandler::sendNotFound(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
        std::string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        boost::asio::write(*socket, boost::asio::buffer(response));
        Debug::Log::info("Sent 404 Not Found response", "RequestHandler");
    }

} // namespace Network