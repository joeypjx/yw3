#ifndef LOGGING_UTILS_HPP
#define LOGGING_UTILS_HPP

#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace monitoring::utils {

/**
 * 日志工具类
 * 提供带时间戳的日志输出功能
 */
class Logger {
public:
    enum Level {
        INFO,
        WARN,
        ERROR,
        DEBUG
    };

    static void log(Level level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::string levelStr;
        switch (level) {
            case INFO:  levelStr = "INFO "; break;
            case WARN:  levelStr = "WARN "; break;
            case ERROR: levelStr = "ERROR"; break;
            case DEBUG: levelStr = "DEBUG"; break;
        }

        std::cout << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
                  << "." << std::setfill('0') << std::setw(3) << ms.count()
                  << "] [" << levelStr << "] " << message << std::endl;
    }

    static void info(const std::string& message) { log(INFO, message); }
    static void warn(const std::string& message) { log(WARN, message); }
    static void error(const std::string& message) { log(ERROR, message); }
    static void debug(const std::string& message) { log(DEBUG, message); }
};

} // namespace monitoring::utils

#endif // LOGGING_UTILS_HPP
