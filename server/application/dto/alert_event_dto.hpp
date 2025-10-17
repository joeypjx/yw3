#ifndef APPLICATION_DTO_ALERT_EVENT_DTO_HPP
#define APPLICATION_DTO_ALERT_EVENT_DTO_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace monitoring::application {

/**
 * AlertEventDTO - 告警事件数据传输对象
 */
struct AlertEventDTO {
    int64_t eventId;                    // 事件ID
    int32_t ruleId;                     // 规则ID
    std::string nodeId;                 // 节点ID
    std::string status;                 // 状态：FIRING, ACKNOWLEDGED, RESOLVED
    std::string severity;               // 严重等级：WARNING, CRITICAL
    uint64_t startAt;                   // 开始时间
    uint64_t endAt;                     // 结束时间（0表示未结束）
    double triggeredValue;              // 触发时的指标值
    std::string details;                // 详细描述
    std::string acknowledgedBy;         // 认知操作员ID
    uint64_t acknowledgedAt;            // 认知时间（0表示未认知）
};

/**
 * AlertEventDetailResponseDTO - 告警事件详情响应
 */
struct AlertEventDetailResponseDTO {
    int code;                           // 响应码
    std::string message;                // 响应消息
    AlertEventDTO event;                // 告警事件详情
};

/**
 * AlertEventListResponseDTO - 告警事件列表响应
 */
struct AlertEventListResponseDTO {
    int code;                           // 响应码
    std::string message;                // 响应消息
    std::vector<AlertEventDTO> events;  // 告警事件列表
    int total;                          // 总数
};

} // namespace monitoring::application

#endif // APPLICATION_DTO_ALERT_EVENT_DTO_HPP
