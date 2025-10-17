#include "mock_alert_rule_repository.hpp"
#include <algorithm>

namespace monitoring::infrastructure {

void MockAlertRuleRepository::save(const domain::AlertRule& rule) {
    std::lock_guard<std::mutex> lock(mutex_);
    rules_[rule.getRuleId()] = std::make_unique<domain::AlertRule>(rule);
}

std::optional<domain::AlertRule> MockAlertRuleRepository::findById(int32_t ruleId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rules_.find(ruleId);
    if (it != rules_.end()) {
        return *(it->second);
    }
    return std::nullopt;
}

std::vector<domain::AlertRule> MockAlertRuleRepository::findAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertRule> result;
    for (const auto& pair : rules_) {
        result.push_back(*(pair.second));
    }
    return result;
}

std::vector<domain::AlertRule> MockAlertRuleRepository::findAllActive() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertRule> result;
    for (const auto& pair : rules_) {
        if (pair.second->isEnabled()) {
            result.push_back(*(pair.second));
        }
    }
    return result;
}

bool MockAlertRuleRepository::remove(int32_t ruleId) {
    std::lock_guard<std::mutex> lock(mutex_);
    return rules_.erase(ruleId) > 0;
}

} // namespace monitoring::infrastructure
