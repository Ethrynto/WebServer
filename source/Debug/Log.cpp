#include "Log.h"
#include <fstream>
#include <iostream>

namespace Debug {

    std::vector<std::string> Log::logs;
    std::mutex Log::mutex_;

    std::string Log::getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::string ts = std::ctime(&time);
        ts.pop_back(); // Remove newline
        return ts;
    }

    void Log::log(Level level, const std::string& message, const std::string& file) {
        std::string prefix;
        switch (level) {
            case Level::Info: prefix = "[INFO]"; break;
            case Level::Warning: prefix = "[WARN]"; break;
            case Level::Error: prefix = "[ERROR]"; break;
        }

        std::string result = std::format("[{}] {} {}", getTimestamp(), prefix, message);
        if (!file.empty()) result = std::format("[{}] {} [{}] {}", getTimestamp(), prefix, file, message);

        {
            std::lock_guard<std::mutex> guard(mutex_);
            logs.push_back(result);
            std::cout << result << std::endl;
            writeToFile(result);
        }
    }

    void Log::writeToFile(const std::string& message) {
        std::ofstream logFile("app_logs.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
        }
    }

} // namespace Debug