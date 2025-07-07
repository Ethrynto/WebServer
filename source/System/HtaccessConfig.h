#ifndef WEBSERVER_HTACCESSCONFIG_H
#define WEBSERVER_HTACCESSCONFIG_H

#include <string>
#include <optional>

namespace System {

    class HtaccessConfig {
    public:
        std::optional<int> port; // Port number specified in .htaccess
        // Future fields: MIME types, environment variables, etc.

        // Parse .htaccess file and return config
        static HtaccessConfig parse(const std::string& filePath);
    };

} // System

#endif //WEBSERVER_HTACCESSCONFIG_H
