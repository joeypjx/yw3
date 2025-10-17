#ifndef INFRASTRUCTURE_NOTIFIERS_MOCK_NOTIFICATION_SERVICE_HPP
#define INFRASTRUCTURE_NOTIFIERS_MOCK_NOTIFICATION_SERVICE_HPP

#include "../../domain/services/i_notification_service.hpp"
#include <vector>
#include <mutex>

namespace monitoring::infrastructure {

/**
 * MockNotificationService - 模拟通知服务
 * 用于测试，将通知记录到内存中
 */
class MockNotificationService : public domain::INotificationService {
public:
    void sendAlertTriggered(const domain::AlertEvent& event) override;
    void sendAlertResolved(const domain::AlertEvent& event) override;
    void sendAlertAcknowledged(const domain::AlertEvent& event) override;

    // 测试辅助方法
    const std::vector<domain::AlertEvent>& getTriggeredAlerts() const;
    const std::vector<domain::AlertEvent>& getResolvedAlerts() const;
    const std::vector<domain::AlertEvent>& getAcknowledgedAlerts() const;
    void clear();

private:
    std::vector<domain::AlertEvent> triggeredAlerts_;
    std::vector<domain::AlertEvent> resolvedAlerts_;
    std::vector<domain::AlertEvent> acknowledgedAlerts_;
    std::mutex mutex_;
};

} // namespace monitoring::infrastructure

#endif // INFRASTRUCTURE_NOTIFIERS_MOCK_NOTIFICATION_SERVICE_HPP
