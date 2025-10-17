#include "mock_alert_event_repository.hpp"
#include <algorithm>

namespace monitoring::infrastructure {

void MockAlertEventRepository::save(const domain::AlertEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    events_[event.getEventId()] = std::make_unique<domain::AlertEvent>(event);
}

std::optional<domain::AlertEvent> MockAlertEventRepository::findById(int64_t eventId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = events_.find(eventId);
    if (it != events_.end()) {
        return *(it->second);
    }
    return std::nullopt;
}

std::vector<domain::AlertEvent> MockAlertEventRepository::findByNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertEvent> result;
    for (const auto& pair : events_) {
        if (pair.second->getNodeId() == nodeId) {
            result.push_back(*(pair.second));
        }
    }
    return result;
}

std::vector<domain::AlertEvent> MockAlertEventRepository::findByRule(int32_t ruleId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertEvent> result;
    for (const auto& pair : events_) {
        if (pair.second->getRuleId() == ruleId) {
            result.push_back(*(pair.second));
        }
    }
    return result;
}

std::optional<domain::AlertEvent> MockAlertEventRepository::findActiveEvent(
    int32_t ruleId, 
    const std::string& nodeId
) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& pair : events_) {
        const auto& event = *(pair.second);
        if (event.getRuleId() == ruleId && 
            event.getNodeId() == nodeId && 
            event.isActive()) {
            return event;
        }
    }
    return std::nullopt;
}

std::vector<domain::AlertEvent> MockAlertEventRepository::findAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertEvent> result;
    for (const auto& pair : events_) {
        result.push_back(*(pair.second));
    }
    return result;
}

std::vector<domain::AlertEvent> MockAlertEventRepository::findAllActive() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertEvent> result;
    for (const auto& pair : events_) {
        if (pair.second->isActive()) {
            result.push_back(*(pair.second));
        }
    }
    return result;
}

std::vector<domain::AlertEvent> MockAlertEventRepository::findByStatus(domain::AlertEvent::Status status) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertEvent> result;
    for (const auto& pair : events_) {
        if (pair.second->getStatus() == status) {
            result.push_back(*(pair.second));
        }
    }
    return result;
}

std::vector<domain::AlertEvent> MockAlertEventRepository::findByTimeRange(
    uint64_t startTime, 
    uint64_t endTime
) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::AlertEvent> result;
    for (const auto& pair : events_) {
        const auto& event = *(pair.second);
        if (event.getStartAt() >= startTime && event.getStartAt() <= endTime) {
            result.push_back(event);
        }
    }
    
    // 按开始时间排序
    std::sort(result.begin(), result.end(),
        [](const domain::AlertEvent& a, const domain::AlertEvent& b) {
            return a.getStartAt() < b.getStartAt();
        });
    
    return result;
}

bool MockAlertEventRepository::remove(int64_t eventId) {
    std::lock_guard<std::mutex> lock(mutex_);
    return events_.erase(eventId) > 0;
}

} // namespace monitoring::infrastructure
