#include "mock_notification_service.hpp"

namespace monitoring::infrastructure {

void MockNotificationService::sendAlertTriggered(const domain::AlertEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    triggeredAlerts_.push_back(event);
}

void MockNotificationService::sendAlertResolved(const domain::AlertEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    resolvedAlerts_.push_back(event);
}

void MockNotificationService::sendAlertAcknowledged(const domain::AlertEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    acknowledgedAlerts_.push_back(event);
}

const std::vector<domain::AlertEvent>& MockNotificationService::getTriggeredAlerts() const {
    return triggeredAlerts_;
}

const std::vector<domain::AlertEvent>& MockNotificationService::getResolvedAlerts() const {
    return resolvedAlerts_;
}

const std::vector<domain::AlertEvent>& MockNotificationService::getAcknowledgedAlerts() const {
    return acknowledgedAlerts_;
}

void MockNotificationService::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    triggeredAlerts_.clear();
    resolvedAlerts_.clear();
    acknowledgedAlerts_.clear();
}

} // namespace monitoring::infrastructure
