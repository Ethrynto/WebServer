#include "Log.h"

#include <iostream>
#include <ctime>

namespace Debug {
    std::string Log::currentDateTime() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

        return buf;
    }

    void Log::alert(const std::string& message, const std::string& file)
    {
        std::cout << "[" << Log::currentDateTime() << "] " << message;
        if (file != "")
            std::cout << " in " << file << std::endl;

        std::cout << std::endl;
    }

    void Log::error(const std::string& message, const std::string& file)
    {
        std::cerr << "[" << Log::currentDateTime() << "] " << message;
        if (file != "")
            std::cerr << " in " << file << std::endl;

        std::cerr << std::endl;
    }

    void Log::warn(const std::string& message, const std::string& file)
    {
        std::cerr << "[" << Log::currentDateTime() << "] " << message;
        if (file != "")
            std::cerr << " in " << file << std::endl;

        std::cerr << std::endl;
    }
} // System