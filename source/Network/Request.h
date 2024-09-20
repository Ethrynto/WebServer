#ifndef REQUEST_H
#define REQUEST_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind/bind.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

using BoostTCP = boost::asio::ip::tcp;

namespace Network
{
    class HttpServer; // forward declaration

    class Request : public boost::enable_shared_from_this<Request>
    {
        HttpServer& server;
        boost::asio::streambuf request;
        boost::asio::streambuf response;

        static std::string readFile(const std::string& path);
        void afterRead(const boost::system::error_code& ec, std::size_t bytes_transferred);
        void afterWrite(const boost::system::error_code& ec, std::size_t bytes_transferred);

    public:
        std::shared_ptr<BoostTCP::socket> socket;
        Request(HttpServer& server);
        void answer();
    };
};

#endif // REQUEST_H
