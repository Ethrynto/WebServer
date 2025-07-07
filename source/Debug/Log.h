#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <mutex>
#include <chrono>

namespace Debug {

    class Log {
    public:
        enum class Level { Info, Warning, Error };

        // Log a message with specified level
        static void log(Level level, const std::string& message, const std::string& file = "");

        // Convenience methods
        static void info(const std::string& message, const std::string& file = "") {
            log(Level::Info, message, file);
        }
        static void warn(const std::string& message, const std::string& file = "") {
            log(Level::Warning, message, file);
        }
        static void error(const std::string& message, const std::string& file = "") {
            log(Level::Error, message, file);
        }

        static std::vector<std::string> logs;

    private:
        static std::string getTimestamp();
        static void writeToFile(const std::string& message);
        static std::mutex mutex_;
    };

} // namespace Debug

#endif // LOG_H