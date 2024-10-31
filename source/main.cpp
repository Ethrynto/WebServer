#include <iostream>
#include <vector>

#include "Network/WebServer.h"

int main()
{
    try {
        boost::asio::io_context ioContext;

        std::vector<std::pair<int, std::string>> projects = {
            {8080, DOMAINS_PATH "/Test"},
            {8081, DOMAINS_PATH "/Test2"},
        };

        Network::WebServer server(ioContext, projects);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}