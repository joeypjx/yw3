#ifndef DOMAIN_ENTITIES_ALERT_RULE_HPP
#define DOMAIN_ENTITIES_ALERT_RULE_HPP

#include <string>
#include <cstdint>
#include <vector>
#include "server_node.hpp"
#include "metric_snapshot.hpp"

namespace monitoring::domain {

/**
 * 告警规则评估结果
 */
enum class EvaluationStatus {
    OK,         // 状态正常，无事发生
    TRIGGER,    // 条件满足，应该触发告警
    RESOLVED    // 条件已不再满足，应该解决现有告警
};

/**
 * 评估结果详情
 */
struct EvaluationResult {
    EvaluationStatus status;
    std::string reason;         // 解释为什么是这个状态
    double triggeredValue;      // 触发或解决时的具体数值
};

/**
 * AlertRule - 告警规则实体
 * 定义监控指标的阈值和触发条件
 */
class AlertRule {
public:
    /**
     * 告警严重等级
     */
    enum class Severity {
        WARNING,    // 警告级别
        CRITICAL    // 严重级别
    };

    /**
     * 比较操作符
     */
    enum class Operator {
        GREATER_THAN,      // >
        LESS_THAN,         // <
        EQUAL,             // =
        GREATER_OR_EQUAL,  // >=
        LESS_OR_EQUAL      // <=
    };

    // 构造函数
    AlertRule(
        int32_t ruleId,
        const std::string& ruleName,
        const std::string& metricName,
        double threshold,
        Operator op,
        int32_t durationSeconds,
        Severity severity,
        bool isEnabled = true
    );

    // 完整构造函数（包含描述和时间戳）
    AlertRule(
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
    );

    // Getters
    int32_t getRuleId() const { return ruleId_; }
    const std::string& getRuleName() const { return ruleName_; }
    const std::string& getMetricName() const { return metricName_; }
    double getThreshold() const { return threshold_; }
    Operator getOperator() const { return operator_; }
    int32_t getDuration() const { return durationSeconds_; }
    Severity getSeverity() const { return severity_; }
    bool isEnabled() const { return isEnabled_; }
    const std::string& getDescription() const { return description_; }
    uint64_t getCreatedAt() const { return createdAt_; }
    uint64_t getUpdatedAt() const { return updatedAt_; }

    // 业务方法

    /**
     * 评估告警规则
     * 这是核心的决策函数，无状态、无副作用
     *
     * @param node 被评估的服务器节点
     * @param recentMetrics 最近的指标快照列表（从旧到新排序）
     * @return 评估结果
     */
    EvaluationResult evaluate(
        const ServerNode& node,
        const std::vector<MetricSnapshot>& recentMetrics
    ) const;

    /**
     * 启用/禁用规则
     */
    void setEnabled(bool enabled) { isEnabled_ = enabled; }

    /**
     * 设置更新时间
     */
    void setUpdatedAt(uint64_t timestamp) { updatedAt_ = timestamp; }

private:
    int32_t ruleId_;              // 规则ID
    std::string ruleName_;        // 规则名称
    std::string metricName_;      // 监控的指标名（如 "cpu.usage_percent"）
    double threshold_;            // 阈值
    Operator operator_;           // 比较操作符
    int32_t durationSeconds_;     // 必须持续多久才触发（秒）
    Severity severity_;           // 严重等级
    bool isEnabled_;              // 是否启用
    std::string description_;     // 规则描述
    uint64_t createdAt_;          // 创建时间
    uint64_t updatedAt_;          // 更新时间

    // 辅助方法：检查单个值是否违反阈值
    bool violatesThreshold(double value) const;

    // 辅助方法：检查数据是否足够新鲜
    bool isDataFresh(const std::vector<MetricSnapshot>& metrics, uint64_t maxAgeSeconds = 60) const;

    // 辅助方法：检查数据是否覆盖所需时间窗口
    bool hasEnoughDataCoverage(const std::vector<MetricSnapshot>& metrics) const;
};

// 辅助函数：枚举转字符串
std::string severityToString(AlertRule::Severity severity);
AlertRule::Severity stringToSeverity(const std::string& str);

std::string operatorToString(AlertRule::Operator op);
AlertRule::Operator stringToOperator(const std::string& str);

} // namespace monitoring::domain

#endif // DOMAIN_ENTITIES_ALERT_RULE_HPP
