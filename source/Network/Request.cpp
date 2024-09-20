#include "Request.h"
#include "HttpServer.h"

Network::Request::Request(boost::asio::io_service& io_service)
{
    socket.reset(new BoostTCP::socket(io_service));
}

// Method for read the HTML-file
std::string Network::Request::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return ""; // If file not found, return the empty string
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void Network::Request::handleReadCompletion(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (!ec) {
        if (bytes_transferred > 0) {
            std::istream request_stream(&request);
            std::string request_line;
            std::getline(request_stream, request_line);

            // Extracting the requested path
            std::string path = request_line.substr(4, request_line.find(' ', 4) - 4); // To get path from the request
            if (path == "/") {
                path = "/index.html"; // If request the root, usage index.html
            }

            // Read HTML-file
            std::string content = readFile(DOMAINS_PATH + path);

            if (!content.empty()) {
                std::ostream res_stream(&response);
                res_stream << "HTTP/1.0 200 OK\r\n"
                    << "Content-Type: text/html; charset=UTF-8\r\n"
                    << "Content-Length: " << content.length() << "\r\n\r\n"
                    << content;

                if (socket) {
                    boost::asio::async_write(*socket, response, boost::bind(&Request::handleWriteCompletion, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2));
                }
            }
            else {
                // If we couldt find file, return 404
                std::ostream res_stream(&response);
                res_stream << "HTTP/1.0 404 Not Found\r\n"
                    << "Content-Type: text/html; charset=UTF-8\r\n"
                    << "Content-Length: 0\r\n\r\n";
                socket->close();
            }
        }
        else {
            std::cerr << "No data received." << std::endl;
        }
    }
    else {
        std::cerr << "Error reading: " << ec.message() << std::endl;
    }
}


void Network::Request::handleWriteCompletion(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    socket->close();
}

void Network::Request::processRequest()
{
    if (!socket) return;

    boost::asio::async_read_until(*socket, request, "\r\n\r\n",
        boost::bind(&Network::Request::handleReadCompletion, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2));
}
