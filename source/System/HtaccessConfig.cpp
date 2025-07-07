#include "HtaccessConfig.h"

#include "../Debug/Log.h"
#include <fstream>
#include <sstream>
#include <string>

namespace System {
    HtaccessConfig HtaccessConfig::parse(const std::string& filePath) {
        HtaccessConfig config;
        std::ifstream file(filePath);
        if (!file) {
            Debug::Log::error(std::format("Failed to open .htaccess: {}", filePath), "HtaccessConfig");
            return config;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Skip empty lines or comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::stringstream ss(line);
            std::string directive;
            ss >> directive;

            if (directive == "Port") {
                int port;
                if (ss >> port && port > 0 && port <= 65535) {
                    config.port = port;
                    Debug::Log::info(std::format("Parsed port {} from .htaccess: {}", port, filePath), "HtaccessConfig");
                } else {
                    Debug::Log::error(std::format("Invalid port in .htaccess: {}", line), "HtaccessConfig");
                }
            } else if (directive == "AddType") {
                std::string extension, mimeType;
                if (ss >> extension >> mimeType) {
                    config.mimeTypes[extension] = mimeType;
                    Debug::Log::info(std::format("Parsed MIME type {} for extension {} from .htaccess: {}", mimeType, extension, filePath), "HtaccessConfig");
                } else {
                    Debug::Log::error(std::format("Invalid AddType in .htaccess: {}", line), "HtaccessConfig");
                }
            }
        }

        return config;
    }
} // System