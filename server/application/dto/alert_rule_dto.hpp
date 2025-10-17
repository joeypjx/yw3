#ifndef APPLICATION_DTO_ALERT_RULE_DTO_HPP
#define APPLICATION_DTO_ALERT_RULE_DTO_HPP

#include <string>
#include <vector>

namespace monitoring::application {

/**
 * 告警规则DTO - 用于API交互
 */
struct AlertRuleDTO {
    int32_t ruleId;
    std::string ruleName;
    std::string metricName;
    double threshold;
    std::string operator_;  // ">", "<", "=", ">=", "<="
    int32_t durationSeconds;
    std::string severity;   // "WARNING", "CRITICAL"
    bool isEnabled;
    std::string description;
    uint64_t createdAt;
    uint64_t updatedAt;
};

/**
 * 创建告警规则请求DTO
 */
struct CreateAlertRuleRequestDTO {
    std::string ruleName;
    std::string metricName;
    double threshold;
    std::string operator_;
    int32_t durationSeconds;
    std::string severity;
    bool isEnabled = true;
    std::string description;
};

/**
 * 更新告警规则请求DTO
 */
struct UpdateAlertRuleRequestDTO {
    std::string ruleName;
    std::string metricName;
    double threshold;
    std::string operator_;
    int32_t durationSeconds;
    std::string severity;
    bool isEnabled;
    std::string description;
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
