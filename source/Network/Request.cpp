#include "Request.h"
#include "HttpServer.h"

std::string Network::Request::TestMessage()
{
    
    std::time_t now = std::time(0);
    std::string result = "Hello world! Test message. Local time: ";
    result.append(std::ctime(&now));
    return result;
}

Network::Request::Request(HttpServer& server) : server(server)
{
    socket.reset(new BoostTCP::socket(server.io_service));
}

void Network::Request::afterRead(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (!ec) {
        if (bytes_transferred > 0) {
            std::ostream res_stream(&response);
            std::string message = TestMessage();

            res_stream << "HTTP/1.0 200 OK\r\n"
                << "Content-Type: text/html; charset=UTF-8\r\n"
                << "Content-Length: " << message.length() + 32 << "\r\n\r\n"
                << message << "\r\n";

            if (socket) {
                boost::asio::async_write(*socket, response, boost::bind(&Network::Request::afterWrite, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2));
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

void Network::Request::afterWrite(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    socket->close();
}

void Network::Request::answer()
{
    if (!socket) return;

    boost::asio::async_read_until(*socket, request, "\r\n\r\n",
        boost::bind(&Network::Request::afterRead, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2));
}
