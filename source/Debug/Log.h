#ifndef LOG_H
#define LOG_H

#include <string>

namespace Debug
{
    class Log
    {
    private:
        static std::string currentDateTime();

    public:
        static void alert(const std::string& message, const std::string& file = "");
        static void warn(const std::string& message, const std::string& file = "");
        static void error(const std::string& message, const std::string& file = "");
    };
} // System

#endif //LOG_H
