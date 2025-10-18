#include "query_service.hpp"
#include "../dto/dto_converter.hpp"
#include "../../domain/entities/server_node.hpp"
#include "../../domain/entities/metric_snapshot.hpp"
#include "../../domain/entities/alert_event.hpp"
#include <algorithm>
#include <chrono>

namespace monitoring::application {

QueryService::QueryService(
    domain::IServerNodeRepository* nodeRepository,
    domain::IMetricRepository* metricRepository,
    domain::IAlertEventRepository* alertEventRepository
)
    : nodeRepository_(nodeRepository)
    , metricRepository_(metricRepository)
    , alertEventRepository_(alertEventRepository)
{
}

std::vector<NodeHealthDTO> QueryService::getNodes() {
    auto nodes = nodeRepository_->findAll();
    std::vector<NodeHealthDTO> result;
    
    for (const auto& node : nodes) {
        result.push_back(DTOConverter::toNodeHealthDTO(node));
    }
    
    return result;
}

std::optional<NodeHealthDTO> QueryService::getNodeById(const std::string& nodeId) {
    auto node = nodeRepository_->findById(nodeId);
    if (node.has_value()) {
        return DTOConverter::toNodeHealthDTO(*node);
    }
    return std::nullopt;
}

std::vector<NodeHealthDTO> QueryService::getNodesByStatus(const std::string& status) {
    // 转换字符串状态为枚举
    domain::ServerNode::Status nodeStatus;
    if (status == "HEALTHY") nodeStatus = domain::ServerNode::Status::HEALTHY;
    else if (status == "WARNING") nodeStatus = domain::ServerNode::Status::WARNING;
    else if (status == "CRITICAL") nodeStatus = domain::ServerNode::Status::CRITICAL;
    else nodeStatus = domain::ServerNode::Status::UNKNOWN;
    
    auto nodes = nodeRepository_->findByStatus(nodeStatus);
    std::vector<NodeHealthDTO> result;
    
    for (const auto& node : nodes) {
        result.push_back(DTOConverter::toNodeHealthDTO(node));
    }
    
    return result;
}

std::vector<MetricDataDTO> QueryService::getMetrics(
    const std::string& nodeId,
    uint64_t startTime,
    uint64_t endTime
) {
    auto snapshots = metricRepository_->findByNodeAndTimeRange(nodeId, startTime, endTime);
    std::vector<MetricDataDTO> result;
    
    for (const auto& snapshot : snapshots) {
        result.push_back(DTOConverter::toMetricDataDTO(snapshot));
    }
    
    return result;
}

std::vector<MetricDataDTO> QueryService::getRecentMetrics(
    const std::string& nodeId,
    int32_t recentSeconds
) {
    auto snapshots = metricRepository_->findRecent(nodeId, recentSeconds);
    std::vector<MetricDataDTO> result;
    
    for (const auto& snapshot : snapshots) {
        result.push_back(DTOConverter::toMetricDataDTO(snapshot));
    }
    
    return result;
}

std::vector<AlertEventDTO> QueryService::getAlertEvents() {
    auto events = alertEventRepository_->findAllActive();
    std::vector<AlertEventDTO> result;
    
    for (const auto& event : events) {
        result.push_back(DTOConverter::toAlertEventDTO(event));
    }
    
    return result;
}

std::vector<AlertEventDTO> QueryService::getAlertEventsByNode(const std::string& nodeId) {
    auto events = alertEventRepository_->findByNode(nodeId);
    std::vector<AlertEventDTO> result;
    
    for (const auto& event : events) {
        result.push_back(DTOConverter::toAlertEventDTO(event));
    }
    
    return result;
}

std::vector<AlertEventDTO> QueryService::getAlertEventsByStatus(const std::string& status) {
    // 转换字符串状态为枚举
    domain::AlertEvent::Status eventStatus;
    if (status == "FIRING") eventStatus = domain::AlertEvent::Status::FIRING;
    else if (status == "ACKNOWLEDGED") eventStatus = domain::AlertEvent::Status::ACKNOWLEDGED;
    else if (status == "RESOLVED") eventStatus = domain::AlertEvent::Status::RESOLVED;
    else return {}; // 无效状态
    
    auto events = alertEventRepository_->findByStatus(eventStatus);
    std::vector<AlertEventDTO> result;
    
    for (const auto& event : events) {
        result.push_back(DTOConverter::toAlertEventDTO(event));
    }
    
    return result;
}

std::vector<AlertEventDTO> QueryService::getActiveAlertEvents() {
    auto events = alertEventRepository_->findAllActive();
    std::vector<AlertEventDTO> result;
    
    for (const auto& event : events) {
        result.push_back(DTOConverter::toAlertEventDTO(event));
    }
    
    return result;
}

} // namespace monitoring::application
