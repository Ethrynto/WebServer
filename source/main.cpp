#include <iostream>
#include <vector>

#include "Debug/Log.h"
#include "Network/WebServer.h"
#include "System/FileManager.h"

int main()
{
    try {
        boost::asio::io_context ioContext;
        int port = 8080;

        std::vector<std::pair<int, std::string>> projects = {};

        for (const auto& folder : System::FileManager::getFolders(DOMAINS_PATH))
        {
            projects.emplace_back(port, DOMAINS_PATH "/" + folder);
            port++;
        }

        const Network::WebServer server(ioContext, projects);
        server.start();
    } catch (const std::exception& e) {
        Debug::Log::alert(std::format("Exception: {}", e.what()));
    }

    return 0;
}
