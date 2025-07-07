#include "RequestHandler.h"
#include "../System/HtaccessConfig.h"
#include "../Debug/Log.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <stdio.h> // For popen/pclose

namespace Network {

    RequestHandler::RequestHandler(boost::asio::io_context& ioContext, const std::string& rootDir)
            : ioContext_(ioContext), rootDir_(rootDir) {
        // Parse .htaccess for MIME types
        htaccessConfig_ = System::HtaccessConfig::parse((std::filesystem::path(rootDir_) / ".htaccess").string());
    }

    void RequestHandler::handleRequest(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
        Debug::Log::info("Handling new request", "RequestHandler");
        try {
            boost::asio::streambuf request;
            boost::system::error_code error;
            boost::asio::read_until(*socket, request, "\r\n\r\n", error);

            if (error && error != boost::asio::error::eof) {
                Debug::Log::error(std::format("Error reading request: {}", error.message()), "RequestHandler");
                return;
            }

            std::stringstream requestStream;
            requestStream << &request;

            std::string requestLine;
            std::getline(requestStream, requestLine);
            Debug::Log::info(std::format("Received {} request for {}",
                                         requestLine.substr(0, requestLine.find(' ')),
                                         requestLine.substr(requestLine.find(' ') + 1,
                                                            requestLine.find(' ', requestLine.find(' ') + 1) - requestLine.find(' ') - 1)),
                             "RequestHandler");

            std::string path = requestLine.substr(requestLine.find(' ') + 1,
                                                  requestLine.find(' ', requestLine.find(' ') + 1) - requestLine.find(' ') - 1);
            if (path == "/") path = "/index.html";

            std::stringstream responseStream;
            std::string contentType = "text/html"; // Default MIME type
            std::filesystem::path filePath = std::filesystem::path(rootDir_) / path.substr(1);

            // Check MIME type from .htaccess
            std::string extension = filePath.extension().string();
            if (!extension.empty() && htaccessConfig_.mimeTypes.count(extension)) {
                contentType = htaccessConfig_.mimeTypes[extension];
                Debug::Log::info(std::format("Using MIME type {} for extension {}", contentType, extension), "RequestHandler");
            }

            responseStream << "HTTP/1.1 200 OK\r\nContent-Type: " << contentType << "\r\nConnection: close\r\n\r\n";

            if (!std::filesystem::exists(filePath)) {
                filePath = std::filesystem::path(rootDir_) / "index.php";
                if (!std::filesystem::exists(filePath)) {
                    responseStream.str("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<h1>404 Not Found</h1>");
                    Debug::Log::error(std::format("File not found: {}", filePath.string()), "RequestHandler");
                }

            } else if (filePath.extension() == ".php") {
                handlePhpRequest(filePath.string(), requestStream, responseStream);
            } else {
                serveStaticFile(filePath.string(), responseStream);
            }

            boost::asio::write(*socket, boost::asio::buffer(responseStream.str()), error);
            if (error) {
                Debug::Log::error(std::format("Error writing response: {}", error.message()), "RequestHandler");
            }
        } catch (const std::exception& e) {
            Debug::Log::error(std::format("Error handling request: {}", e.what()), "RequestHandler");
        }
    }

    void RequestHandler::serveStaticFile(const std::string& path, std::stringstream& responseStream) {
        std::ifstream file(path, std::ios::binary);
        if (file) {
            std::stringstream content;
            content << file.rdbuf();
            responseStream << content.str();
            Debug::Log::info(std::format("Served static file: {}", path), "RequestHandler");
        } else {
            responseStream.str("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<h1>404 Not Found</h1>");
            Debug::Log::error(std::format("Failed to open file: {}", path), "RequestHandler");
        }
    }

    void RequestHandler::handlePhpRequest(const std::string& path, std::stringstream& requestStream, std::stringstream& responseStream) {
        // Check if PHP is available
        std::string phpCommand = std::string(
#ifdef _WIN32
                "php --version"
#else
                "php-cgi --version"
#endif
        );
        if (std::system((phpCommand + " >nul 2>&1").c_str()) != 0) {
            responseStream.str("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<h1>500 Internal Server Error</h1><p>PHP is not installed or not found in PATH.</p>");
            Debug::Log::error("PHP is not installed or not found in PATH", "RequestHandler");
            return;
        }

        // Prepare PHP command
        std::string command = std::string(
#ifdef _WIN32
                "php \""
#else
                "php-cgi \""
#endif
        ) + path + "\"";

#ifdef _WIN32
        FILE* pipe = _popen(command.c_str(), "r");
#else
        FILE* pipe = popen(command.c_str(), "r");
#endif
        if (!pipe) {
            responseStream.str("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<h1>500 Internal Server Error</h1>");
            Debug::Log::error(std::format("Failed to execute PHP script: {}", path), "RequestHandler");
            return;
        }

        char buffer[128];
        std::stringstream phpOutput;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            phpOutput << buffer;
        }

#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif

        responseStream << phpOutput.str();
        Debug::Log::info(std::format("Served PHP file: {}", path), "RequestHandler");
    }

} // namespace Network