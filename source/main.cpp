#include <iostream>
#include <vector>

#include "System/FileManager.h"
#include "Network/HttpServer.h"

int main(int argc, char* argv[])
{
    System::FileManager* domains = new System::FileManager(DOMAINS_PATH);
    int serverPort = 8080;

    std::vector<Network::HttpServer*> servers;
    servers.push_back(new Network::HttpServer(8080, "Test\\"));
    servers.push_back(new Network::HttpServer(8081, "Test2\\"));
    servers[0]->start();
    servers[1]->start();

    delete domains;
}
