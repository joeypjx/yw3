#ifndef DOMAIN_SERVICES_ALERT_EXPRESSION_EVALUATION_SERVICE_HPP
#define DOMAIN_SERVICES_ALERT_EXPRESSION_EVALUATION_SERVICE_HPP

#include <string>
#include <vector>
#include <map>
#include "../entities/metric_snapshot.hpp"
#include "../entities/server_node.hpp"

namespace monitoring::domain {

/**
 * 告警条件 - 单个监控条件
 */
struct AlertCondition {
    std::string metric;           // 监控指标名称
    std::string operator_;       // 比较运算符: ">", "<", ">=", "<=", "==", "!="
    double threshold;            // 阈值
    std::string duration;        // 持续时间: "1m", "5m", "1h" 等
    std::map<std::string, std::string> tags;  // 标签选择器（可选）
};

/**
 * 告警表达式 - 包含多个条件和逻辑关系
 */
struct AlertExpression {
    std::vector<AlertCondition> conditions;  // 条件数组
    std::string logic;                      // 逻辑关系: "AND", "OR"
    std::map<std::string, std::string> tags; // 全局标签（可选）
};

/**
 * 告警表达式评估结果
 */
struct AlertEvaluationResult {
    bool triggered;                    // 是否触发告警
    double triggeredValue;            // 触发的值
    std::string reason;               // 触发原因
    std::vector<std::string> matchedConditions;  // 匹配的条件
    std::vector<std::string> failedConditions;  // 失败的条件
};

/**
 * 告警表达式评估服务
 * 
 * 领域服务 - 负责评估多条件告警表达式，支持AND/OR逻辑
 * 这是监控系统的核心业务逻辑
 */
class AlertExpressionEvaluationService {
public:
    /**
     * 评估告警表达式
     * 
     * @param expression 告警表达式
     * @param metrics 指标快照
     * @param node 服务器节点（包含完整的节点信息）
     * @return 评估结果
     */
    static AlertEvaluationResult evaluate(
        const AlertExpression& expression,
        const MetricSnapshot& metrics,
        const ServerNode& node
    );

private:
    /**
     * 评估单个条件
     * 
     * @param condition 告警条件
     * @param metrics 指标快照
     * @param globalTags 全局标签（当条件没有自己的标签时使用）
     * @param node 服务器节点（包含完整的节点信息）
     * @return 是否满足条件
     */
    static bool evaluateCondition(
        const AlertCondition& condition,
        const MetricSnapshot& metrics,
        const std::map<std::string, std::string>& globalTags,
        const ServerNode& node
    );

    /**
     * 检查标签匹配
     *
     * @param tags 条件标签
     * @param metrics 指标快照
     * @param node 服务器节点（包含完整的节点信息）
     * @return 是否匹配
     */
    static bool matchTags(
        const std::map<std::string, std::string>& tags,
        const MetricSnapshot& metrics,
        const ServerNode& node
    );

    /**
     * 比较两个值
     *
     * @param value 实际值
     * @param threshold 阈值
     * @param op 操作符
     * @return 比较结果
     */
    static bool compareValues(double value, double threshold, const std::string& op);
};

} // namespace monitoring::domain

#endif // DOMAIN_SERVICES_ALERT_EXPRESSION_EVALUATION_SERVICE_HPP
