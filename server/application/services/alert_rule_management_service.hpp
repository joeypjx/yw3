#ifndef APPLICATION_SERVICES_ALERT_RULE_MANAGEMENT_SERVICE_HPP
#define APPLICATION_SERVICES_ALERT_RULE_MANAGEMENT_SERVICE_HPP

#include <string>
#include <vector>
#include <memory>
#include "../../domain/repositories/i_alert_rule_repository.hpp"
#include "../dto/alert_rule_dto.hpp"
#include "../../domain/services/alert_expression_evaluation_service.hpp"
#include "../../common/id_generator.hpp"

namespace monitoring::application {

/**
 * AlertRuleManagementService - 告警规则管理服务
 *
 * 职责：
 * 1. 创建告警规则
 * 2. 更新告警规则
 * 3. 删除告警规则
 * 4. 查询告警规则列表
 * 5. 获取告警规则详情
 * 6. 启用/禁用告警规则
 */
class AlertRuleManagementService {
public:
    /**
     * 构造函数 - 依赖注入
     */
    AlertRuleManagementService(domain::IAlertRuleRepository* alertRuleRepository);

    /**
     * 创建告警规则
     */
    AlertRuleDetailResponseDTO createAlertRule(const CreateAlertRuleRequestDTO& request);

    /**
     * 更新告警规则
     */
    AlertRuleDetailResponseDTO updateAlertRule(int32_t ruleId, const UpdateAlertRuleRequestDTO& request);

    /**
     * 删除告警规则
     */
    CommonResponseDTO deleteAlertRule(int32_t ruleId);

    /**
     * 获取告警规则列表
     */
    AlertRuleListResponseDTO getAlertRules();

    /**
     * 获取告警规则详情
     */
    AlertRuleDetailResponseDTO getAlertRule(int32_t ruleId);

    /**
     * 启用告警规则
     */
    CommonResponseDTO enableAlertRule(int32_t ruleId);

    /**
     * 禁用告警规则
     */
    CommonResponseDTO disableAlertRule(int32_t ruleId);

    /**
     * 获取启用的告警规则列表
     */
    AlertRuleListResponseDTO getActiveAlertRules();

private:
    domain::IAlertRuleRepository* alertRuleRepository_;

    // 辅助方法：转换领域对象为DTO
    AlertRuleDTO toAlertRuleDTO(const domain::AlertRule& rule);

    // 辅助方法：转换DTO为领域对象
    domain::AlertRule toDomainAlertRule(const CreateAlertRuleRequestDTO& dto, int32_t ruleId);
    domain::AlertRule toDomainAlertRule(const UpdateAlertRuleRequestDTO& dto, int32_t ruleId);

    // 辅助方法：验证请求参数
    bool validateCreateRequest(const CreateAlertRuleRequestDTO& request, std::string& errorMessage);
    bool validateUpdateRequest(const UpdateAlertRuleRequestDTO& request, std::string& errorMessage);

    // 辅助方法：生成下一个规则ID
    int32_t generateNextRuleId();
};

} // namespace monitoring::application

#endif // APPLICATION_SERVICES_ALERT_RULE_MANAGEMENT_SERVICE_HPP
