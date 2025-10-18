#include "alert_event.hpp"
#include <chrono>

namespace monitoring::domain {

AlertEvent AlertEvent::create(
    int64_t eventId,
    const AlertRule& rule,
    const ServerNode& node,
    double triggeredValue,
    const std::string& details
) {
    return AlertEvent(
        eventId,
        rule.getRuleId(),
        node.getNodeId(),
        rule.getSeverity(),
        triggeredValue,
        details,
        rule.getAlertName(),
        rule.getDescription(),
        rule.getSummary(),
        rule.getAlertType(),
        rule.getExpression(),
        node.getIpAddress()
    );
}

AlertEvent::AlertEvent(
    int64_t eventId,
    int32_t ruleId,
    const std::string& nodeId,
    const std::string& severity,
    double triggeredValue,
    const std::string& details,
    const std::string& alertName,
    const std::string& description,
    const std::string& summary,
    const std::string& alertType,
    const AlertExpression& expression,
    const std::string& nodeIpAddress
)
    : eventId_(eventId)
    , ruleId_(ruleId)
    , nodeId_(nodeId)
    , status_(Status::FIRING)  // 新建的告警事件初始状态为 FIRING
    , severity_(severity)
    , startAt_(getCurrentTimestamp())
    , endAt_(0)
    , triggeredValue_(triggeredValue)
    , details_(details)
    , acknowledgedBy_("")
    , acknowledgedAt_(0)
    , alertName_(alertName)
    , description_(description)
    , summary_(summary)
    , alertType_(alertType)
    , expression_(expression)
    , nodeIpAddress_(nodeIpAddress)
{
}

void AlertEvent::acknowledge(const std::string& operatorId) {
    // 业务规则：只有 Firing 状态的告警才能被认知
    if (status_ != Status::FIRING) {
        throw std::logic_error("Alert event can only be acknowledged when it is firing.");
    }

    status_ = Status::ACKNOWLEDGED;
    acknowledgedBy_ = operatorId;
    acknowledgedAt_ = getCurrentTimestamp();
}

void AlertEvent::resolve() {
    // 幂等性：如果已经是 RESOLVED 状态，什么都不做
    if (status_ == Status::RESOLVED) {
        return;
    }

    // 业务规则：只有 Firing 或 Acknowledged 状态才能解决
    if (status_ != Status::FIRING && status_ != Status::ACKNOWLEDGED) {
        throw std::logic_error("Alert event can only be resolved from FIRING or ACKNOWLEDGED state.");
    }

    status_ = Status::RESOLVED;
    endAt_ = getCurrentTimestamp();
}

uint64_t AlertEvent::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

// 辅助函数实现
std::string alertStatusToString(AlertEvent::Status status) {
    switch (status) {
        case AlertEvent::Status::FIRING:       return "FIRING";
        case AlertEvent::Status::ACKNOWLEDGED: return "ACKNOWLEDGED";
        case AlertEvent::Status::RESOLVED:     return "RESOLVED";
        default: return "UNKNOWN";
    }
}

AlertEvent::Status stringToAlertStatus(const std::string& str) {
    if (str == "FIRING")       return AlertEvent::Status::FIRING;
    if (str == "ACKNOWLEDGED") return AlertEvent::Status::ACKNOWLEDGED;
    if (str == "RESOLVED")     return AlertEvent::Status::RESOLVED;
    throw std::invalid_argument("Unknown alert status: " + str);
}

} // namespace monitoring::domain
