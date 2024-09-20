#include "HttpServer.h"
#include "Request.h"

Network::HttpServer::HttpServer(unsigned int port) : acceptor(io_service, BoostTCP::endpoint(BoostTCP::v4(), port)) {}

Network::HttpServer::~HttpServer()
{
    if (sThread) sThread->join();
}

void Network::HttpServer::start()
{
    sThread.reset(new std::thread(boost::bind(&Network::HttpServer::runServiceLoop, this)));
}

void Network::HttpServer::runServiceLoop()
{
    beginAcceptingConnections();
    io_service.run();
}

void Network::HttpServer::beginAcceptingConnections()
{
    boost::shared_ptr<Request> req(new Network::Request(this->io_service));
    acceptor.async_accept(*req->socket,
        boost::bind(&Network::HttpServer::handleNewConnection, this, req, boost::placeholders::_1));
}

void Network::HttpServer::handleNewConnection(boost::shared_ptr<Request> req, const boost::system::error_code& error)
{
    if (!error) {
        req->processRequest();
    }
    beginAcceptingConnections();
}
