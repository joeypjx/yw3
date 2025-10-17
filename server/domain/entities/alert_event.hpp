#ifndef DOMAIN_ENTITIES_ALERT_EVENT_HPP
#define DOMAIN_ENTITIES_ALERT_EVENT_HPP

#include <string>
#include <cstdint>
#include <stdexcept>
#include <memory>
#include "alert_rule.hpp"
#include "server_node.hpp"

namespace monitoring::domain {

/**
 * AlertEvent - 告警事件实体
 * 代表一个已触发的告警，拥有自己的生命周期和状态
 * 这是一个富领域对象，封装了状态转换的业务规则
 */
class AlertEvent {
public:
    /**
     * 告警事件状态
     */
    enum class Status {
        FIRING,         // 正在触发
        ACKNOWLEDGED,   // 已被认知（已知晓但未解决）
        RESOLVED        // 已解决
    };

    /**
     * 工厂方法：创建新的告警事件
     * 确保对象创建时状态有效
     */
    static AlertEvent create(
        int64_t eventId,
        const AlertRule& rule,
        const ServerNode& node,
        double triggeredValue,
        const std::string& details
    );

    // Getters
    int64_t getEventId() const { return eventId_; }
    int32_t getRuleId() const { return ruleId_; }
    const std::string& getNodeId() const { return nodeId_; }
    Status getStatus() const { return status_; }
    AlertRule::Severity getSeverity() const { return severity_; }
    uint64_t getStartAt() const { return startAt_; }
    uint64_t getEndAt() const { return endAt_; }
    double getTriggeredValue() const { return triggeredValue_; }
    const std::string& getDetails() const { return details_; }
    const std::string& getAcknowledgedBy() const { return acknowledgedBy_; }
    uint64_t getAcknowledgedAt() const { return acknowledgedAt_; }

    /**
     * 业务行为：认知告警
     * 规则：只有状态为 Firing 的告警才能被认知
     *
     * @param operatorId 操作员ID
     * @throws std::logic_error 如果当前状态不是 Firing
     */
    void acknowledge(const std::string& operatorId);

    /**
     * 业务行为：解决告警
     * 规则：只有 Firing 或 Acknowledged 状态的告警才能被解决
     * 解决操作是幂等的，重复解决不会报错
     */
    void resolve();

    /**
     * 判断告警是否处于活跃状态（未解决）
     */
    bool isActive() const {
        return status_ == Status::FIRING || status_ == Status::ACKNOWLEDGED;
    }

private:
    // 私有构造函数，强制使用工厂方法
    AlertEvent(
        int64_t eventId,
        int32_t ruleId,
        const std::string& nodeId,
        AlertRule::Severity severity,
        double triggeredValue,
        const std::string& details
    );

    int64_t eventId_;               // 事件ID（主键）
    int32_t ruleId_;                // 关联的规则ID
    std::string nodeId_;            // 关联的节点ID
    Status status_;                 // 当前状态
    AlertRule::Severity severity_;  // 严重等级（规则的快照）

    uint64_t startAt_;              // 告警开始时间
    uint64_t endAt_;                // 告警结束时间（0表示未结束）

    double triggeredValue_;         // 触发时的指标值
    std::string details_;           // 详细描述

    std::string acknowledgedBy_;    // 认知操作员ID
    uint64_t acknowledgedAt_;       // 认知时间（0表示未认知）

    // 获取当前Unix时间戳（秒）
    static uint64_t getCurrentTimestamp();
};

// 辅助函数：状态转字符串
std::string alertStatusToString(AlertEvent::Status status);
AlertEvent::Status stringToAlertStatus(const std::string& str);

} // namespace monitoring::domain

#endif // DOMAIN_ENTITIES_ALERT_EVENT_HPP
