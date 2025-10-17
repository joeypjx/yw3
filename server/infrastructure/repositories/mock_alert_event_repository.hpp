#ifndef INFRASTRUCTURE_REPOSITORIES_MOCK_ALERT_EVENT_REPOSITORY_HPP
#define INFRASTRUCTURE_REPOSITORIES_MOCK_ALERT_EVENT_REPOSITORY_HPP

#include "../../domain/repositories/i_alert_event_repository.hpp"
#include <map>
#include <mutex>
#include <vector>

namespace monitoring::infrastructure {

/**
 * MockAlertEventRepository - 内存模拟告警事件仓储
 * 用于测试，数据存储在内存中
 */
class MockAlertEventRepository : public domain::IAlertEventRepository {
public:
    void save(const domain::AlertEvent& event) override;
    std::optional<domain::AlertEvent> findById(int64_t eventId) override;
    std::vector<domain::AlertEvent> findByNode(const std::string& nodeId) override;
    std::vector<domain::AlertEvent> findByRule(int32_t ruleId) override;
    std::optional<domain::AlertEvent> findActiveEvent(
        int32_t ruleId, 
        const std::string& nodeId
    ) override;
    std::vector<domain::AlertEvent> findAll() override;
    std::vector<domain::AlertEvent> findAllActive() override;
    std::vector<domain::AlertEvent> findByStatus(domain::AlertEvent::Status status) override;
    std::vector<domain::AlertEvent> findByTimeRange(
        uint64_t startTime, 
        uint64_t endTime
    ) override;
    bool remove(int64_t eventId) override;

private:
    std::map<int64_t, std::unique_ptr<domain::AlertEvent>> events_;
    std::mutex mutex_;
};

} // namespace monitoring::infrastructure

#endif // INFRASTRUCTURE_REPOSITORIES_MOCK_ALERT_EVENT_REPOSITORY_HPP
