#ifndef COMMON_TIME_UTILS_HPP
#define COMMON_TIME_UTILS_HPP

#include <string>
#include <cstdint>
#include <stdexcept>

namespace monitoring::common {

/**
 * TimeUtils - 时间相关的工具函数
 * 提供统一的时间解析和转换功能
 */
class TimeUtils {
public:
    /**
     * 解析持续时间字符串为秒数
     *
     * 支持的格式:
     *   - "60s" -> 60秒
     *   - "5m"  -> 300秒
     *   - "2h"  -> 7200秒
     *   - "1d"  -> 86400秒
     *
     * @param duration 持续时间字符串 (例如: "5m", "1h")
     * @return 转换后的秒数
     * @throws std::invalid_argument 如果格式无效
     */
    static int32_t parseDurationToSeconds(const std::string& duration) {
        if (duration.empty()) {
            return DEFAULT_DURATION_SECONDS;
        }

        // 提取单位和数值
        char unit = duration.back();
        std::string valueStr = duration.substr(0, duration.length() - 1);

        if (valueStr.empty()) {
            throw std::invalid_argument("Invalid duration format: " + duration);
        }

        int32_t value;
        try {
            value = std::stoi(valueStr);
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid duration value: " + duration);
        }

        // 根据单位转换
        switch (unit) {
            case 's':
                return value;
            case 'm':
                return value * SECONDS_PER_MINUTE;
            case 'h':
                return value * SECONDS_PER_HOUR;
            case 'd':
                return value * SECONDS_PER_DAY;
            default:
                throw std::invalid_argument("Unknown duration unit: " + std::string(1, unit));
        }
    }

    /**
     * 检查持续时间格式是否有效
     *
     * @param duration 持续时间字符串
     * @return 是否有效
     */
    static bool isValidDurationFormat(const std::string& duration) {
        if (duration.empty()) {
            return false;
        }

        char unit = duration.back();
        if (unit != 's' && unit != 'm' && unit != 'h' && unit != 'd') {
            return false;
        }

        std::string valueStr = duration.substr(0, duration.length() - 1);
        if (valueStr.empty()) {
            return false;
        }

        try {
            std::stoi(valueStr);
            return true;
        } catch (...) {
            return false;
        }
    }

    // 时间常量
    static constexpr int32_t SECONDS_PER_MINUTE = 60;
    static constexpr int32_t SECONDS_PER_HOUR = 3600;
    static constexpr int32_t SECONDS_PER_DAY = 86400;
    static constexpr int32_t DEFAULT_DURATION_SECONDS = 60;
    static constexpr int32_t DEFAULT_DATA_FRESHNESS_SECONDS = 300;  // 5分钟
};

} // namespace monitoring::common

#endif // COMMON_TIME_UTILS_HPP
