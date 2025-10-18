#ifndef PRESENTATION_CONTROLLERS_SIMPLE_JSON_PARSER_HPP
#define PRESENTATION_CONTROLLERS_SIMPLE_JSON_PARSER_HPP

#include <string>
#include <map>
#include <vector>
#include "../../application/dto/heartbeat_dto.hpp"
#include "../../application/dto/resource_dto.hpp"
#include "../../application/dto/alert_rule_dto.hpp"
#include "../../application/dto/alert_event_dto.hpp"
#include "json.hpp"

namespace monitoring::presentation {

/**
 * SimpleJsonParser - JSON解析器
 *
 * 使用 nlohmann/json 库实现生产级JSON解析
 * 
 * 功能：
 * - 解析Agent发送的JSON请求
 * - 生成响应JSON
 * - 支持完整的JSON语法和错误处理
 */
class SimpleJsonParser {
public:
    /**
     * 解析心跳请求JSON
     */
    static application::HeartbeatRequestDTO parseHeartbeatRequest(const std::string& json);

    /**
     * 解析资源上报请求JSON
     */
    static application::ResourceReportRequestDTO parseResourceRequest(const std::string& json);

    /**
     * 生成心跳响应JSON
     */
    static std::string serializeHeartbeatResponse(const application::HeartbeatResponseDTO& dto);

    /**
     * 生成资源上报响应JSON
     */
    static std::string serializeResourceResponse(const application::ResourceReportResponseDTO& dto);

    // ============ 告警规则管理API相关方法 ============

    /**
     * 解析创建告警规则请求JSON
     */
    static application::CreateAlertRuleRequestDTO parseCreateAlertRuleRequest(const std::string& json);

    /**
     * 解析更新告警规则请求JSON
     */
    static application::UpdateAlertRuleRequestDTO parseUpdateAlertRuleRequest(const std::string& json);

    /**
     * 生成告警规则详情响应JSON
     */
    static std::string serializeAlertRuleDetailResponse(const application::AlertRuleDetailResponseDTO& dto);

    /**
     * 生成告警规则列表响应JSON
     */
    static std::string serializeAlertRuleListResponse(const application::AlertRuleListResponseDTO& dto);

    /**
     * 生成通用响应JSON
     */
    static std::string serializeCommonResponse(const application::CommonResponseDTO& dto);

    // ============ 告警事件序列化方法 ============
    
    /**
     * 序列化告警事件详情响应
     */
    static std::string serializeAlertEventDetailResponse(const application::AlertEventDetailResponseDTO& dto);
    
    /**
     * 序列化告警事件列表响应
     */
    static std::string serializeAlertEventListResponse(const application::AlertEventListResponseDTO& dto);

private:
    // 辅助函数：提取JSON字段值
    static std::string extractStringField(const std::string& json, const std::string& field);
    static int32_t extractIntField(const std::string& json, const std::string& field);
    static uint16_t extractUint16Field(const std::string& json, const std::string& field);
    static double extractDoubleField(const std::string& json, const std::string& field);
    static uint64_t extractUint64Field(const std::string& json, const std::string& field);
    static bool extractBoolField(const std::string& json, const std::string& field);

    // 转义JSON字符串
    static std::string escapeJson(const std::string& str);
};

} // namespace monitoring::presentation

#endif // PRESENTATION_CONTROLLERS_SIMPLE_JSON_PARSER_HPP
