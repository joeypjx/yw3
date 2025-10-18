#include "alert_rule.hpp"
#include "../services/alert_expression_evaluation_service.hpp"
#include <chrono>
#include <algorithm>
#include <iostream>

namespace monitoring::domain {

AlertRule::AlertRule(
    int32_t ruleId,
    const std::string& alertName,
    const AlertExpression& expression,
    const std::string& severity,
    bool isEnabled,
    const std::string& description,
    const std::string& alertType,
    const std::string& summary
) : ruleId_(ruleId), alertName_(alertName), expression_(expression), 
    severity_(severity), isEnabled_(isEnabled), description_(description),
    alertType_(alertType), summary_(summary) {
    
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    createdAt_ = static_cast<uint64_t>(timestamp);
    updatedAt_ = createdAt_;
}

EvaluationResult AlertRule::evaluate(const ServerNode& node, const std::vector<MetricSnapshot>& recentMetrics) const {
    EvaluationResult result;
    
    if (!isEnabled_) {
        result.status = EvaluationStatus::OK;
        result.reason = "Rule is disabled";
        result.triggeredValue = 0.0;
        return result;
    }
    
    // 检查数据新鲜度
    if (!isDataFresh(recentMetrics)) {
        result.status = EvaluationStatus::OK;
        result.reason = "Data is not fresh enough";
        result.triggeredValue = 0.0;
        return result;
    }
    
    // 检查数据覆盖度
    if (!hasEnoughDataCoverage(recentMetrics)) {
        result.status = EvaluationStatus::OK;
        result.reason = "Not enough data coverage";
        result.triggeredValue = 0.0;
        return result;
    }
    
    // 使用最新的指标数据进行评估
    const MetricSnapshot& latestMetrics = recentMetrics.back();
    
    // 使用表达式评估器进行评估
    auto evaluationResult = AlertExpressionEvaluationService::evaluate(
        expression_, latestMetrics, node.getNodeId()
    );
    
    if (evaluationResult.triggered) {
        result.status = EvaluationStatus::TRIGGER;
        result.triggeredValue = evaluationResult.triggeredValue;
        result.reason = evaluationResult.reason + " - " + alertName_;
        
        std::cout << "[AlertRule] Rule " << alertName_ 
                  << " triggered for node " << node.getNodeId() 
                  << " with value " << result.triggeredValue << std::endl;
    } else {
        result.status = EvaluationStatus::OK;
        result.triggeredValue = 0.0;
        result.reason = evaluationResult.reason + " - " + alertName_;
    }
    
    return result;
}

bool AlertRule::isDataFresh(const std::vector<MetricSnapshot>& metrics) const {
    if (metrics.empty()) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    const MetricSnapshot& latest = metrics.back();
    uint64_t timeDiff = static_cast<uint64_t>(currentTime) - latest.getTimestamp();

    return timeDiff <= common::TimeUtils::DEFAULT_DATA_FRESHNESS_SECONDS;
}

bool AlertRule::hasEnoughDataCoverage(const std::vector<MetricSnapshot>& metrics) const {
    if (metrics.empty()) {
        return false;
    }
    
    int32_t minDuration = getMinDurationSeconds();
    
    if (metrics.size() == 1) {
        // 如果只有一个数据点，检查是否满足最小持续时间
        return minDuration <= 1; // 对于1秒的持续时间，一个数据点就足够了
    }
    
    // 检查时间跨度是否足够
    const MetricSnapshot& oldest = metrics.front();
    const MetricSnapshot& latest = metrics.back();
    
    uint64_t timeSpan = latest.getTimestamp() - oldest.getTimestamp();
    
    return timeSpan >= static_cast<uint64_t>(minDuration);
}

void AlertRule::updateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    updatedAt_ = static_cast<uint64_t>(timestamp);
}

int32_t AlertRule::getMinDurationSeconds() const {
    int32_t minDuration = common::TimeUtils::DEFAULT_DURATION_SECONDS;

    for (const auto& condition : expression_.conditions) {
        try {
            int32_t duration = common::TimeUtils::parseDurationToSeconds(condition.duration);
            minDuration = std::min(minDuration, duration);
        } catch (const std::exception& e) {
            std::cerr << "[AlertRule] Error parsing duration '" << condition.duration
                      << "': " << e.what() << ", using default" << std::endl;
        }
    }

    return minDuration;
}

} // namespace monitoring::domain
