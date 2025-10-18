#ifndef APPLICATION_SERVICES_ALERT_EVENT_MANAGEMENT_SERVICE_HPP
#define APPLICATION_SERVICES_ALERT_EVENT_MANAGEMENT_SERVICE_HPP

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "../dto/alert_event_dto.hpp"
#include "../dto/alert_rule_dto.hpp"
#include "../../domain/repositories/i_alert_event_repository.hpp"

namespace monitoring::application {

/**
 * AlertEventManagementService - 告警事件管理服务
 * 提供告警事件的查询、状态管理等功能
 */
class AlertEventManagementService {
public:
    /**
     * 构造函数
     * @param alertEventRepository 告警事件仓储
     */
    explicit AlertEventManagementService(
        std::shared_ptr<domain::IAlertEventRepository> alertEventRepository
    );

    /**
     * 获取所有告警事件
     * @return 告警事件列表响应
     */
    AlertEventListResponseDTO getAllAlertEvents();

    /**
     * 根据ID获取告警事件详情
     * @param eventId 事件ID
     * @return 告警事件详情响应
     */
    AlertEventDetailResponseDTO getAlertEventById(int64_t eventId);

    /**
     * 根据节点ID获取告警事件
     * @param nodeId 节点ID
     * @return 告警事件列表响应
     */
    AlertEventListResponseDTO getAlertEventsByNode(const std::string& nodeId);

    /**
     * 根据规则ID获取告警事件
     * @param ruleId 规则ID
     * @return 告警事件列表响应
     */
    AlertEventListResponseDTO getAlertEventsByRule(int32_t ruleId);

    /**
     * 获取活跃的告警事件（FIRING 或 ACKNOWLEDGED）
     * @return 告警事件列表响应
     */
    AlertEventListResponseDTO getActiveAlertEvents();

    /**
     * 根据状态获取告警事件
     * @param status 告警状态
     * @return 告警事件列表响应
     */
    AlertEventListResponseDTO getAlertEventsByStatus(const std::string& status);

    /**
     * 认知告警事件
     * @param eventId 事件ID
     * @param operatorId 操作员ID
     * @return 通用响应
     */
    CommonResponseDTO acknowledgeAlertEvent(int64_t eventId, const std::string& operatorId);

    /**
     * 解决告警事件
     * @param eventId 事件ID
     * @return 通用响应
     */
    CommonResponseDTO resolveAlertEvent(int64_t eventId);

private:
    std::shared_ptr<domain::IAlertEventRepository> alertEventRepository_;
};

} // namespace monitoring::application

#endif // APPLICATION_SERVICES_ALERT_EVENT_MANAGEMENT_SERVICE_HPP
