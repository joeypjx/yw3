#ifndef DOMAIN_REPOSITORIES_I_ALERT_RULE_REPOSITORY_HPP
#define DOMAIN_REPOSITORIES_I_ALERT_RULE_REPOSITORY_HPP

#include <string>
#include <vector>
#include <optional>
#include "../entities/alert_rule.hpp"

namespace monitoring::domain {

/**
 * IAlertRuleRepository - 告警规则仓储接口
 * 定义如何持久化和检索 AlertRule
 */
class IAlertRuleRepository {
public:
    virtual ~IAlertRuleRepository() = default;

    /**
     * 保存或更新规则
     * @param rule 告警规则
     */
    virtual void save(const AlertRule& rule) = 0;

    /**
     * 根据规则ID查找规则
     * @param ruleId 规则ID
     * @return 如果找到返回规则，否则返回 std::nullopt
     */
    virtual std::optional<AlertRule> findById(int32_t ruleId) = 0;

    /**
     * 查找所有规则
     * @return 所有规则列表
     */
    virtual std::vector<AlertRule> findAll() = 0;

    /**
     * 查找所有启用的规则
     * @return 启用的规则列表
     */
    virtual std::vector<AlertRule> findAllActive() = 0;

    /**
     * 删除规则
     * @param ruleId 规则ID
     * @return 是否删除成功
     */
    virtual bool remove(int32_t ruleId) = 0;
};

} // namespace monitoring::domain

#endif // DOMAIN_REPOSITORIES_I_ALERT_RULE_REPOSITORY_HPP
