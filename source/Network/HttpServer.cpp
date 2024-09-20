#include "HttpServer.h"
#include "Request.h"

Network::HttpServer::HttpServer(unsigned int port) : acceptor(io_service, BoostTCP::endpoint(BoostTCP::v4(), port)) {}

Network::HttpServer::~HttpServer()
{
    if (sThread) sThread->join();
}

void Network::HttpServer::Run()
{
    sThread.reset(new std::thread(boost::bind(&Network::HttpServer::startThread, this)));
}

void Network::HttpServer::startThread()
{
    startAccept();
    io_service.run();
}

void Network::HttpServer::startAccept()
{
    boost::shared_ptr<Request> req(new Network::Request(*this));
    acceptor.async_accept(*req->socket,
        boost::bind(&Network::HttpServer::handleAccept, this, req, boost::placeholders::_1));
}

void Network::HttpServer::handleAccept(boost::shared_ptr<Request> req, const boost::system::error_code& error)
{
    if (!error) {
        req->answer();
    }
    startAccept();
}
