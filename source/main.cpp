#include <iostream>
#include <vector>

#include "Debug/Log.h"
#include "Network/WebServer.h"

int main()
{
    try {
        boost::asio::io_context ioContext;

        const std::vector<std::pair<int, std::string>> projects = {
            {8080, DOMAINS_PATH "/Test"},
            {8081, DOMAINS_PATH "/Test2"},
        };

        const Network::WebServer server(ioContext, projects);
        server.start();
    } catch (const std::exception& e) {
        Debug::Log::alert(std::format("Exception: {}", e.what()));
    }

    return 0;
}