#ifndef APPLICATION_SERVICES_QUERY_SERVICE_HPP
#define APPLICATION_SERVICES_QUERY_SERVICE_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "../../domain/repositories/i_server_node_repository.hpp"
#include "../../domain/repositories/i_metric_repository.hpp"
#include "../../domain/repositories/i_alert_event_repository.hpp"
#include "../dto/alert_event_dto.hpp"
#include "../dto/query_dto.hpp"

namespace monitoring::application {

/**
 * QueryService - 查询服务
 *
 * 职责：
 * 1. 处理前端API的查询请求
 * 2. 从仓储获取数据
 * 3. 将领域对象转换为DTO
 * 4. 返回给前端展示
 */
class QueryService {
public:
    /**
     * 构造函数 - 依赖注入
     */
    QueryService(
        domain::IServerNodeRepository* nodeRepository,
        domain::IMetricRepository* metricRepository,
        domain::IAlertEventRepository* alertEventRepository
    );

    /**
     * 获取所有节点列表
     */
    std::vector<NodeHealthDTO> getNodes();

    /**
     * 根据ID获取节点详情
     */
    std::optional<NodeHealthDTO> getNodeById(const std::string& nodeId);

    /**
     * 获取指定状态的节点
     */
    std::vector<NodeHealthDTO> getNodesByStatus(const std::string& status);

    /**
     * 获取节点的指标数据
     */
    std::vector<MetricDataDTO> getMetrics(
        const std::string& nodeId,
        uint64_t startTime,
        uint64_t endTime
    );

    /**
     * 获取节点的最近指标数据
     */
    std::vector<MetricDataDTO> getRecentMetrics(
        const std::string& nodeId,
        int32_t recentSeconds = 300
    );

    /**
     * 获取所有告警事件
     */
    std::vector<AlertEventDTO> getAlertEvents();

    /**
     * 获取指定节点的告警事件
     */
    std::vector<AlertEventDTO> getAlertEventsByNode(const std::string& nodeId);

    /**
     * 获取指定状态的告警事件
     */
    std::vector<AlertEventDTO> getAlertEventsByStatus(const std::string& status);

    /**
     * 获取活跃的告警事件
     */
    std::vector<AlertEventDTO> getActiveAlertEvents();

private:
    domain::IServerNodeRepository* nodeRepository_;
    domain::IMetricRepository* metricRepository_;
    domain::IAlertEventRepository* alertEventRepository_;
};

} // namespace monitoring::application

#endif // APPLICATION_SERVICES_QUERY_SERVICE_HPP
