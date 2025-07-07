#include "Log.h"

#include <iostream>
#include <ctime>

namespace Debug {

    static std::mutex logMutex;

    std::string Log::currentDateTime() {
        time_t     now = time(nullptr);
        struct tm  tm{};
        char       buf[80];
        tm = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tm);

        return buf;
    }

    void Log::alert(const std::string& message, const std::string& file)
    {
        std::string result = "[" + Log::currentDateTime() + "]";
        if (!file.empty())
            result += "[" + file + "] ";

        result += message + "\n";
        Log::inputLog(result);
        std::cout << result;
    }

    void Log::error(const std::string& message, const std::string& file)
    {
        std::string result = "[" + Log::currentDateTime() + "]";
        if (!file.empty())
            result += "[" + file + "] ";

        result += message + "\n";
        Log::inputLog(result);
        std::cout << result;
    }

    void Log::warn(const std::string& message, const std::string& file)
    {
        std::string result = "[" + Log::currentDateTime() + "]";
        if (!file.empty())
            result += "[" + file + "] ";

        result += message + "\n";
        Log::inputLog(result);
        std::cout << result;
    }

    void Log::inputLog(const std::string &message) {
        std::lock_guard<std::mutex> guard(logMutex);

        std::ofstream logFile("app_logs.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << message;
            logFile.close();
        }
    }

} // System