#ifndef APPLICATION_DTO_ALERT_RULE_DTO_HPP
#define APPLICATION_DTO_ALERT_RULE_DTO_HPP

#include <string>
#include <vector>
#include <map>

namespace monitoring::application {

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
    std::string stable;                     // 数据域: "node", "component" 等（可选）
    std::map<std::string, std::string> tags; // 全局标签（可选）
};

/**
 * 告警规则DTO - 用于API交互（新格式）
 */
struct AlertRuleDTO {
    int32_t ruleId;
    std::string alert_name;        // 告警规则名称
    std::string alert_type;        // 告警类型标签（可选）
    std::string description;       // 告警描述
    bool enabled;                  // 是否启用
    AlertExpression expression;    // 告警表达式
    std::string severity;          // 严重程度
    std::string summary;           // 告警摘要（可选）
    uint64_t createdAt;
    uint64_t updatedAt;
};

/**
 * 创建告警规则请求DTO
 */
struct CreateAlertRuleRequestDTO {
    std::string alert_name;        // 告警规则名称
    std::string alert_type;        // 告警类型标签（可选）
    std::string description;       // 告警描述
    bool enabled = true;           // 是否启用
    AlertExpression expression;    // 告警表达式
    std::string severity;          // 严重程度
    std::string summary;           // 告警摘要（可选）
};

/**
 * 更新告警规则请求DTO
 */
struct UpdateAlertRuleRequestDTO {
    std::string alert_name;        // 告警规则名称
    std::string alert_type;        // 告警类型标签（可选）
    std::string description;       // 告警描述
    bool enabled;                 // 是否启用
    AlertExpression expression;    // 告警表达式
    std::string severity;          // 严重程度
    std::string summary;           // 告警摘要（可选）
};

/**
 * 告警规则列表响应DTO
 */
struct AlertRuleListResponseDTO {
    int32_t code;
    std::string message;
    std::vector<AlertRuleDTO> rules;
    int32_t total;
};

/**
 * 告警规则详情响应DTO
 */
struct AlertRuleDetailResponseDTO {
    int32_t code;
    std::string message;
    AlertRuleDTO rule;
};

/**
 * 通用响应DTO
 */
struct CommonResponseDTO {
    int32_t code;
    std::string message;
};

} // namespace monitoring::application

#endif // APPLICATION_DTO_ALERT_RULE_DTO_HPP