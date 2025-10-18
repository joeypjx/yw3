#ifndef DOMAIN_ENTITIES_ALERT_RULE_HPP
#define DOMAIN_ENTITIES_ALERT_RULE_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include "server_node.hpp"
#include "metric_snapshot.hpp"
#include "../services/alert_expression_evaluation_service.hpp"
#include "../../common/time_utils.hpp"

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
 * 告警规则实体
 * 
 * 支持复杂的多条件表达式评估
 */
class AlertRule {
public:
    /**
     * 构造函数
     */
    AlertRule(
        int32_t ruleId,
        const std::string& alertName,
        const AlertExpression& expression,
        const std::string& severity,
        bool isEnabled = true,
        const std::string& description = "",
        const std::string& alertType = "",
        const std::string& summary = ""
    );

    // Getters
    int32_t getRuleId() const { return ruleId_; }
    const std::string& getAlertName() const { return alertName_; }
    const AlertExpression& getExpression() const { return expression_; }
    const std::string& getSeverity() const { return severity_; }
    bool isEnabled() const { return isEnabled_; }
    const std::string& getDescription() const { return description_; }
    const std::string& getAlertType() const { return alertType_; }
    const std::string& getSummary() const { return summary_; }
    uint64_t getCreatedAt() const { return createdAt_; }
    uint64_t getUpdatedAt() const { return updatedAt_; }

    // Setters
    void setEnabled(bool enabled) { isEnabled_ = enabled; updateTimestamp(); }
    void setDescription(const std::string& description) { description_ = description; updateTimestamp(); }
    void setSummary(const std::string& summary) { summary_ = summary; updateTimestamp(); }

    /**
     * 评估告警规则
     * 
     * @param node 目标节点
     * @param recentMetrics 最近一段时间内的指标数据
     * @return 评估结果
     */
    EvaluationResult evaluate(const ServerNode& node, const std::vector<MetricSnapshot>& recentMetrics) const;

    /**
     * 检查数据是否足够新鲜
     */
    bool isDataFresh(const std::vector<MetricSnapshot>& metrics) const;

    /**
     * 检查是否有足够的数据覆盖
     */
    bool hasEnoughDataCoverage(const std::vector<MetricSnapshot>& metrics) const;

private:
    int32_t ruleId_;
    std::string alertName_;
    AlertExpression expression_;
    std::string severity_;
    bool isEnabled_;
    std::string description_;
    std::string alertType_;
    std::string summary_;
    uint64_t createdAt_;
    uint64_t updatedAt_;

    /**
     * 更新时间戳
     */
    void updateTimestamp();

    /**
     * 获取表达式的最小持续时间（秒）
     */
    int32_t getMinDurationSeconds() const;

};

} // namespace monitoring::domain

#endif // DOMAIN_ENTITIES_ALERT_RULE_HPP
