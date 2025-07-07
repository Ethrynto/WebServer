#ifndef WEBSERVER_HTACCESSCONFIG_H
#define WEBSERVER_HTACCESSCONFIG_H

#include <string>
#include <optional>
#include <map>

namespace System {

    class HtaccessConfig {
    public:
        std::optional<int> port; // Port number specified in .htaccess
        std::map<std::string, std::string> mimeTypes; // MIME types for file extensions

        // Parse .htaccess file and return config
        static HtaccessConfig parse(const std::string& filePath);
    };

} // System

#endif //WEBSERVER_HTACCESSCONFIG_H
