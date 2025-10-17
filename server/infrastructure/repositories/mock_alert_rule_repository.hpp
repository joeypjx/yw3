#ifndef INFRASTRUCTURE_REPOSITORIES_MOCK_ALERT_RULE_REPOSITORY_HPP
#define INFRASTRUCTURE_REPOSITORIES_MOCK_ALERT_RULE_REPOSITORY_HPP

#include "../../domain/repositories/i_alert_rule_repository.hpp"
#include <map>
#include <mutex>

namespace monitoring::infrastructure {

/**
 * MockAlertRuleRepository - 内存模拟告警规则仓储
 * 用于测试，数据存储在内存中
 */
class MockAlertRuleRepository : public domain::IAlertRuleRepository {
public:
    void save(const domain::AlertRule& rule) override;
    std::optional<domain::AlertRule> findById(int32_t ruleId) override;
    std::vector<domain::AlertRule> findAll() override;
    std::vector<domain::AlertRule> findAllActive() override;
    bool remove(int32_t ruleId) override;

private:
    std::map<int32_t, std::unique_ptr<domain::AlertRule>> rules_;
    std::mutex mutex_;
};

} // namespace monitoring::infrastructure

#endif // INFRASTRUCTURE_REPOSITORIES_MOCK_ALERT_RULE_REPOSITORY_HPP
