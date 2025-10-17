#include "query_service.hpp"
#include "../../domain/entities/server_node.hpp"
#include "../../domain/entities/metrics.hpp"
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
        result.push_back(toNodeHealthDTO(node));
    }
    
    return result;
}

std::optional<NodeHealthDTO> QueryService::getNodeById(const std::string& nodeId) {
    auto node = nodeRepository_->findById(nodeId);
    if (node.has_value()) {
        return toNodeHealthDTO(*node);
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
        result.push_back(toNodeHealthDTO(node));
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
        result.push_back(toMetricDataDTO(snapshot));
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
        result.push_back(toMetricDataDTO(snapshot));
    }
    
    return result;
}

std::vector<AlertEventDTO> QueryService::getAlertEvents() {
    auto events = alertEventRepository_->findAllActive();
    std::vector<AlertEventDTO> result;
    
    for (const auto& event : events) {
        result.push_back(toAlertEventDTO(event));
    }
    
    return result;
}

std::vector<AlertEventDTO> QueryService::getAlertEventsByNode(const std::string& nodeId) {
    auto events = alertEventRepository_->findByNode(nodeId);
    std::vector<AlertEventDTO> result;
    
    for (const auto& event : events) {
        result.push_back(toAlertEventDTO(event));
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
        result.push_back(toAlertEventDTO(event));
    }
    
    return result;
}

std::vector<AlertEventDTO> QueryService::getActiveAlertEvents() {
    auto events = alertEventRepository_->findAllActive();
    std::vector<AlertEventDTO> result;
    
    for (const auto& event : events) {
        result.push_back(toAlertEventDTO(event));
    }
    
    return result;
}

// 辅助方法实现
NodeHealthDTO QueryService::toNodeHealthDTO(const domain::ServerNode& node) {
    NodeHealthDTO dto;
    dto.nodeId = node.getNodeId();
    dto.hostname = node.getHostname();
    dto.ipAddress = node.getIpAddress();
    dto.status = domain::statusToString(node.getStatus());
    dto.lastSeenAt = node.getLastSeenAt();
    dto.isOnline = node.isOnline();
    return dto;
}

MetricDataDTO QueryService::toMetricDataDTO(const domain::MetricSnapshot& snapshot) {
    MetricDataDTO dto;
    dto.timestamp = snapshot.getTimestamp();
    dto.cpuUsage = snapshot.getCpu().usagePercent;
    dto.memoryUsage = snapshot.getMemory().usagePercent;
    
    // 计算磁盘使用率（取平均值）
    double totalDiskUsage = 0.0;
    const auto& disks = snapshot.getDisks();
    if (!disks.empty()) {
        for (const auto& disk : disks) {
            totalDiskUsage += disk.usagePercent;
        }
        dto.diskUsage = totalDiskUsage / disks.size();
    } else {
        dto.diskUsage = 0.0;
    }
    
    // 计算网络速率（取总和）
    uint64_t totalRxRate = 0, totalTxRate = 0;
    const auto& networks = snapshot.getNetworks();
    for (const auto& network : networks) {
        totalRxRate += network.rxRate;
        totalTxRate += network.txRate;
    }
    dto.networkRxRate = static_cast<double>(totalRxRate);
    dto.networkTxRate = static_cast<double>(totalTxRate);
    
    return dto;
}

AlertEventDTO QueryService::toAlertEventDTO(const domain::AlertEvent& event) {
    AlertEventDTO dto;
    dto.eventId = event.getEventId();
    dto.ruleId = event.getRuleId();
    dto.nodeId = event.getNodeId();
    dto.status = domain::alertStatusToString(event.getStatus());
    dto.severity = domain::severityToString(event.getSeverity());
    dto.startAt = event.getStartAt();
    dto.endAt = event.getEndAt();
    dto.triggeredValue = event.getTriggeredValue();
    dto.details = event.getDetails();
    dto.acknowledgedBy = event.getAcknowledgedBy();
    dto.acknowledgedAt = event.getAcknowledgedAt();
    return dto;
}

} // namespace monitoring::application
