#include "alert_rule_management_service.hpp"
#include "../dto/dto_converter.hpp"
#include "../../domain/entities/alert_rule.hpp"
#include "../../logging_utils.hpp"
#include <chrono>
#include <algorithm>

namespace monitoring::application {

AlertRuleManagementService::AlertRuleManagementService(domain::IAlertRuleRepository* alertRuleRepository)
    : alertRuleRepository_(alertRuleRepository)
{
}

AlertRuleDetailResponseDTO AlertRuleManagementService::createAlertRule(const CreateAlertRuleRequestDTO& request) {
    AlertRuleDetailResponseDTO response;

    try {
        // 1. 验证请求参数
        std::string errorMessage;
        if (!validateCreateRequest(request, errorMessage)) {
            response.code = 400;
            response.message = "Invalid request: " + errorMessage;
            return response;
        }

        // 2. 生成规则ID
        int32_t ruleId = generateNextRuleId();

        // 3. 转换为领域对象
        domain::AlertRule rule = DTOConverter::toAlertRule(request, ruleId);

        // 4. 保存到仓储
        alertRuleRepository_->save(rule);

        // 5. 返回成功响应
        response.code = 0;
        response.message = "Alert rule created successfully";
        response.rule = DTOConverter::toAlertRuleDTO(rule);

        monitoring::utils::Logger::info("Created alert rule: " + std::to_string(ruleId) + " - " + request.alert_name);

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error creating alert rule: " + std::string(e.what()));
    }

    return response;
}

AlertRuleDetailResponseDTO AlertRuleManagementService::updateAlertRule(int32_t ruleId, const UpdateAlertRuleRequestDTO& request) {
    AlertRuleDetailResponseDTO response;

    try {
        // 1. 验证请求参数
        std::string errorMessage;
        if (!validateUpdateRequest(request, errorMessage)) {
            response.code = 400;
            response.message = "Invalid request: " + errorMessage;
            return response;
        }

        // 2. 检查规则是否存在
        auto existingRule = alertRuleRepository_->findById(ruleId);
        if (!existingRule) {
            response.code = 404;
            response.message = "Alert rule not found";
            return response;
        }

        // 3. 转换为领域对象
        domain::AlertRule rule = DTOConverter::toAlertRule(request, ruleId);

        // 4. 保存到仓储
        alertRuleRepository_->save(rule);

        // 5. 返回成功响应
        response.code = 0;
        response.message = "Alert rule updated successfully";
        response.rule = DTOConverter::toAlertRuleDTO(rule);

        monitoring::utils::Logger::info("Updated alert rule: " + std::to_string(ruleId) + " - " + request.alert_name);

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error updating alert rule: " + std::string(e.what()));
    }

    return response;
}

CommonResponseDTO AlertRuleManagementService::deleteAlertRule(int32_t ruleId) {
    CommonResponseDTO response;

    try {
        // 1. 检查规则是否存在
        auto existingRule = alertRuleRepository_->findById(ruleId);
        if (!existingRule) {
            response.code = 404;
            response.message = "Alert rule not found";
            return response;
        }

        // 2. 删除规则
        bool success = alertRuleRepository_->remove(ruleId);
        if (!success) {
            response.code = 500;
            response.message = "Failed to delete alert rule";
            return response;
        }

        // 3. 返回成功响应
        response.code = 0;
        response.message = "Alert rule deleted successfully";

        monitoring::utils::Logger::info("Deleted alert rule: " + std::to_string(ruleId));

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error deleting alert rule: " + std::string(e.what()));
    }

    return response;
}

AlertRuleListResponseDTO AlertRuleManagementService::getAlertRules() {
    AlertRuleListResponseDTO response;

    try {
        // 1. 获取所有规则
        auto rules = alertRuleRepository_->findAll();

        // 2. 转换为DTO
        response.rules.reserve(rules.size());
        for (const auto& rule : rules) {
            response.rules.push_back(DTOConverter::toAlertRuleDTO(rule));
        }

        // 3. 返回成功响应
        response.code = 0;
        response.message = "Alert rules retrieved successfully";
        response.total = static_cast<int32_t>(rules.size());

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error retrieving alert rules: " + std::string(e.what()));
    }

    return response;
}

AlertRuleDetailResponseDTO AlertRuleManagementService::getAlertRule(int32_t ruleId) {
    AlertRuleDetailResponseDTO response;

    try {
        // 1. 查找规则
        auto rule = alertRuleRepository_->findById(ruleId);
        if (!rule) {
            response.code = 404;
            response.message = "Alert rule not found";
            return response;
        }

        // 2. 返回成功响应
        response.code = 0;
        response.message = "Alert rule retrieved successfully";
        response.rule = DTOConverter::toAlertRuleDTO(*rule);

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error retrieving alert rule: " + std::string(e.what()));
    }

    return response;
}

CommonResponseDTO AlertRuleManagementService::enableAlertRule(int32_t ruleId) {
    CommonResponseDTO response;
    std::cout << "[AlertRuleManagementService] enableAlertRule called with ruleId: " << ruleId << std::endl;

    try {
        // 1. 查找规则
        std::cout << "[AlertRuleManagementService] Looking up rule..." << std::endl;
        auto rule = alertRuleRepository_->findById(ruleId);
        if (!rule) {
            std::cout << "[AlertRuleManagementService] Rule not found" << std::endl;
            response.code = 404;
            response.message = "Alert rule not found";
            return response;
        }
        std::cout << "[AlertRuleManagementService] Rule found: " << rule->getAlertName() << std::endl;

        // 2. 启用规则
        std::cout << "[AlertRuleManagementService] Enabling rule..." << std::endl;
        rule->setEnabled(true);
        // 更新updatedAt时间戳
        // 注意：新的AlertRule会在setEnabled中自动更新时间戳
        std::cout << "[AlertRuleManagementService] Saving rule..." << std::endl;
        alertRuleRepository_->save(*rule);

        // 3. 返回成功响应
        response.code = 0;
        response.message = "Alert rule enabled successfully";
        std::cout << "[AlertRuleManagementService] Rule enabled successfully" << std::endl;

        monitoring::utils::Logger::info("Enabled alert rule: " + std::to_string(ruleId));

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error enabling alert rule: " + std::string(e.what()));
    }

    return response;
}

CommonResponseDTO AlertRuleManagementService::disableAlertRule(int32_t ruleId) {
    CommonResponseDTO response;

    try {
        // 1. 查找规则
        auto rule = alertRuleRepository_->findById(ruleId);
        if (!rule) {
            response.code = 404;
            response.message = "Alert rule not found";
            return response;
        }

        // 2. 禁用规则
        rule->setEnabled(false);
        // 更新updatedAt时间戳
        // 注意：新的AlertRule会在setEnabled中自动更新时间戳
        alertRuleRepository_->save(*rule);

        // 3. 返回成功响应
        response.code = 0;
        response.message = "Alert rule disabled successfully";

        monitoring::utils::Logger::info("Disabled alert rule: " + std::to_string(ruleId));

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error disabling alert rule: " + std::string(e.what()));
    }

    return response;
}

AlertRuleListResponseDTO AlertRuleManagementService::getActiveAlertRules() {
    AlertRuleListResponseDTO response;

    try {
        // 1. 获取启用的规则
        auto rules = alertRuleRepository_->findAllActive();

        // 2. 转换为DTO
        response.rules.reserve(rules.size());
        for (const auto& rule : rules) {
            response.rules.push_back(DTOConverter::toAlertRuleDTO(rule));
        }

        // 3. 返回成功响应
        response.code = 0;
        response.message = "Active alert rules retrieved successfully";
        response.total = static_cast<int32_t>(rules.size());

    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        monitoring::utils::Logger::error("Error retrieving active alert rules: " + std::string(e.what()));
    }

    return response;
}

// 辅助方法实现

bool AlertRuleManagementService::validateCreateRequest(const CreateAlertRuleRequestDTO& request, std::string& errorMessage) {
    if (request.alert_name.empty()) {
        errorMessage = "Alert name is required";
        return false;
    }
    if (request.expression.conditions.empty()) {
        errorMessage = "At least one condition is required";
        return false;
    }
    if (request.severity.empty()) {
        errorMessage = "Severity is required";
        return false;
    }
    
    // 验证表达式
    if (request.expression.metric.empty()) {
        errorMessage = "Metric name is required";
        return false;
    }
    if (request.for_.empty()) {
        errorMessage = "Duration (for) is required";
        return false;
    }
    
    // 验证每个条件
    for (const auto& condition : request.expression.conditions) {
        if (condition.threshold < 0) {
            errorMessage = "Threshold must be non-negative for all conditions";
            return false;
        }
        if (condition.operator_ != ">" && condition.operator_ != "<" && condition.operator_ != "==" && 
            condition.operator_ != ">=" && condition.operator_ != "<=" && condition.operator_ != "!=") {
            errorMessage = "Invalid operator: " + condition.operator_;
            return false;
        }
    }
    
    return true;
}

bool AlertRuleManagementService::validateUpdateRequest(const UpdateAlertRuleRequestDTO& request, std::string& errorMessage) {
    if (request.alert_name.empty()) {
        errorMessage = "Alert name is required";
        return false;
    }
    if (request.expression.conditions.empty()) {
        errorMessage = "At least one condition is required";
        return false;
    }
    if (request.severity.empty()) {
        errorMessage = "Severity is required";
        return false;
    }
    
    // 验证表达式
    if (request.expression.metric.empty()) {
        errorMessage = "Metric name is required";
        return false;
    }
    if (request.for_.empty()) {
        errorMessage = "Duration (for) is required";
        return false;
    }
    
    // 验证每个条件
    for (const auto& condition : request.expression.conditions) {
        if (condition.threshold < 0) {
            errorMessage = "Threshold must be non-negative for all conditions";
            return false;
        }
        if (condition.operator_ != ">" && condition.operator_ != "<" && condition.operator_ != "==" && 
            condition.operator_ != ">=" && condition.operator_ != "<=" && condition.operator_ != "!=") {
            errorMessage = "Invalid operator: " + condition.operator_;
            return false;
        }
    }
    
    return true;
}

int32_t AlertRuleManagementService::generateNextRuleId() {
    // 使用线程安全的ID生成器
    return common::IdGenerator::getInstance().nextAlertRuleId();
}

} // namespace monitoring::application
