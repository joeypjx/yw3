#ifndef APPLICATION_DTO_ALERT_EVENT_DTO_HPP
#define APPLICATION_DTO_ALERT_EVENT_DTO_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <map>

namespace monitoring::application {

/**
 * AlertEventAnnotations - 告警事件注释
 */
struct AlertEventAnnotations {
    std::string description;               // 描述
    std::string summary;                   // 摘要
};

/**
 * AlertEventLabels - 告警事件标签
 */
struct AlertEventLabels {
    std::string alert_type;                // 告警类型
    std::string alertname;                 // 告警名称
    std::string host_ip;                   // 主机IP
    std::string metrics;                   // 指标名称
    std::string severity;                  // 严重等级
    std::string value;                     // 触发值
};

/**
 * AlertEventDTO - 告警事件数据传输对象
 */
struct AlertEventDTO {
    AlertEventAnnotations annotations;     // 注释
    std::string created_at;                // 创建时间 (YYYY-MM-DD HH:MM:SS)
    std::string ends_at;                   // 结束时间 (YYYY-MM-DD HH:MM:SS)
    std::string fingerprint;               // 指纹 (ruleId,nodeId组合)
    std::string id;                        // 事件ID (字符串格式)
    AlertEventLabels labels;               // 标签
    std::string starts_at;                 // 开始时间 (YYYY-MM-DD HH:MM:SS)
    std::string status;                    // 状态：firing, acknowledged, resolved
    std::string updated_at;                // 更新时间 (YYYY-MM-DD HH:MM:SS)
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
