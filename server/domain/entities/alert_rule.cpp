#include "alert_rule.hpp"
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace monitoring::domain {

AlertRule::AlertRule(
    int32_t ruleId,
    const std::string& ruleName,
    const std::string& metricName,
    double threshold,
    Operator op,
    int32_t durationSeconds,
    Severity severity,
    bool isEnabled
)
    : ruleId_(ruleId)
    , ruleName_(ruleName)
    , metricName_(metricName)
    , threshold_(threshold)
    , operator_(op)
    , durationSeconds_(durationSeconds)
    , severity_(severity)
    , isEnabled_(isEnabled)
    , description_("")
    , createdAt_(0)
    , updatedAt_(0)
{
}

AlertRule::AlertRule(
    int32_t ruleId,
    const std::string& ruleName,
    const std::string& metricName,
    double threshold,
    Operator op,
    int32_t durationSeconds,
    Severity severity,
    bool isEnabled,
    const std::string& description,
    uint64_t createdAt,
    uint64_t updatedAt
)
    : ruleId_(ruleId)
    , ruleName_(ruleName)
    , metricName_(metricName)
    , threshold_(threshold)
    , operator_(op)
    , durationSeconds_(durationSeconds)
    , severity_(severity)
    , isEnabled_(isEnabled)
    , description_(description)
    , createdAt_(createdAt)
    , updatedAt_(updatedAt)
{
}

EvaluationResult AlertRule::evaluate(
    const ServerNode& node,
    const std::vector<MetricSnapshot>& recentMetrics
) const {
    // 1. 前置检查：是否有足够的数据来进行判断？
    if (recentMetrics.empty()) {
        return { EvaluationStatus::OK, "No metric data available.", 0.0 };
    }

    // 2. 检查数据的新鲜度
    if (!isDataFresh(recentMetrics)) {
        return { EvaluationStatus::OK, "Metric data is stale.", 0.0 };
    }

    // 3. 检查数据是否覆盖所需的时间窗口
    if (!hasEnoughDataCoverage(recentMetrics)) {
        return { EvaluationStatus::OK, "Not enough data to cover the duration window.", 0.0 };
    }

    // 4. 核心评估逻辑：从最新的数据点开始，向回追溯
    bool allPointsViolateThreshold = true;
    double lastValue = 0.0;

    // 我们只关心在 durationSeconds_ 窗口内的数据点
    uint64_t latestTimestamp = recentMetrics.back().getTimestamp();
    uint64_t windowStartTs = latestTimestamp - durationSeconds_;

    for (auto it = recentMetrics.rbegin(); it != recentMetrics.rend(); ++it) {
        const auto& snapshot = *it;

        // 已经超出了我们的时间窗口，停止检查
        if (snapshot.getTimestamp() < windowStartTs) {
            break;
        }

        // 提取指标值
        double currentValue = 0.0;
        try {
            currentValue = snapshot.extractMetricValue(metricName_);
        } catch (const std::exception&) {
            // 如果指标不存在，认为条件不满足
            allPointsViolateThreshold = false;
            break;
        }

        if (it == recentMetrics.rbegin()) { // 记录最新的值
            lastValue = currentValue;
        }

        // 检查是否违反阈值
        if (!violatesThreshold(currentValue)) {
            allPointsViolateThreshold = false;
            break; // 只要有一个点不满足，整个条件就不成立
        }
    }

    // 5. 根据评估结果，返回最终判决
    if (allPointsViolateThreshold) {
        std::ostringstream oss;
        oss << "Metric " << metricName_ << " was "
            << operatorToString(operator_) << " " << threshold_
            << " for at least " << durationSeconds_ << " seconds. "
            << "Current value: " << lastValue;
        return { EvaluationStatus::TRIGGER, oss.str(), lastValue };
    } else {
        std::ostringstream oss;
        oss << "Metric " << metricName_ << " is within the normal range. "
            << "Current value: " << lastValue;
        return { EvaluationStatus::RESOLVED, oss.str(), lastValue };
    }
}

bool AlertRule::violatesThreshold(double value) const {
    switch (operator_) {
        case Operator::GREATER_THAN:
            return value > threshold_;
        case Operator::LESS_THAN:
            return value < threshold_;
        case Operator::EQUAL:
            return value == threshold_;
        case Operator::GREATER_OR_EQUAL:
            return value >= threshold_;
        case Operator::LESS_OR_EQUAL:
            return value <= threshold_;
        default:
            return false;
    }
}

bool AlertRule::isDataFresh(const std::vector<MetricSnapshot>& metrics, uint64_t maxAgeSeconds) const {
    if (metrics.empty()) return false;

    auto now = std::chrono::system_clock::now();
    uint64_t nowEpoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    uint64_t latestTimestamp = metrics.back().getTimestamp();
    return (nowEpoch - latestTimestamp) <= maxAgeSeconds;
}

bool AlertRule::hasEnoughDataCoverage(const std::vector<MetricSnapshot>& metrics) const {
    if (metrics.empty()) return false;

    // 如果只有一个数据点，且durationSeconds为1秒，则认为是足够的
    if (metrics.size() == 1 && durationSeconds_ <= 1) {
        return true;
    }

    uint64_t oldestTs = metrics.front().getTimestamp();
    uint64_t newestTs = metrics.back().getTimestamp();

    return (newestTs - oldestTs) >= static_cast<uint64_t>(durationSeconds_);
}

// 辅助函数实现
std::string severityToString(AlertRule::Severity severity) {
    switch (severity) {
        case AlertRule::Severity::WARNING:  return "WARNING";
        case AlertRule::Severity::CRITICAL: return "CRITICAL";
        default: return "WARNING";
    }
}

AlertRule::Severity stringToSeverity(const std::string& str) {
    if (str == "CRITICAL") return AlertRule::Severity::CRITICAL;
    return AlertRule::Severity::WARNING;
}

std::string operatorToString(AlertRule::Operator op) {
    switch (op) {
        case AlertRule::Operator::GREATER_THAN:     return ">";
        case AlertRule::Operator::LESS_THAN:        return "<";
        case AlertRule::Operator::EQUAL:            return "=";
        case AlertRule::Operator::GREATER_OR_EQUAL: return ">=";
        case AlertRule::Operator::LESS_OR_EQUAL:    return "<=";
        default: return ">";
    }
}

AlertRule::Operator stringToOperator(const std::string& str) {
    if (str == ">")  return AlertRule::Operator::GREATER_THAN;
    if (str == "<")  return AlertRule::Operator::LESS_THAN;
    if (str == "=")  return AlertRule::Operator::EQUAL;
    if (str == ">=") return AlertRule::Operator::GREATER_OR_EQUAL;
    if (str == "<=") return AlertRule::Operator::LESS_OR_EQUAL;
    return AlertRule::Operator::GREATER_THAN;
}

} // namespace monitoring::domain
