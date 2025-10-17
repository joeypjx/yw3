#include "simple_json_parser.hpp"
#include <sstream>
#include <regex>
#include <stdexcept>

namespace monitoring::presentation {

// ============ 辅助函数实现 ============

std::string SimpleJsonParser::extractStringField(const std::string& json, const std::string& field) {
    // 简单的正则提取："field": "value"
    std::string pattern = "\"" + field + "\"\\s*:\\s*\"([^\"]*)\"";
    std::regex re(pattern);
    std::smatch match;

    if (std::regex_search(json, match, re) && match.size() > 1) {
        return match[1].str();
    }
    return "";
}

int32_t SimpleJsonParser::extractIntField(const std::string& json, const std::string& field) {
    // 提取："field": 123
    std::string pattern = "\"" + field + "\"\\s*:\\s*(-?\\d+)";
    std::regex re(pattern);
    std::smatch match;

    if (std::regex_search(json, match, re) && match.size() > 1) {
        return std::stoi(match[1].str());
    }
    return 0;
}

uint16_t SimpleJsonParser::extractUint16Field(const std::string& json, const std::string& field) {
    return static_cast<uint16_t>(extractIntField(json, field));
}

double SimpleJsonParser::extractDoubleField(const std::string& json, const std::string& field) {
    // 提取："field": 123.45
    std::string pattern = "\"" + field + "\"\\s*:\\s*(-?\\d+\\.?\\d*)";
    std::regex re(pattern);
    std::smatch match;

    if (std::regex_search(json, match, re) && match.size() > 1) {
        return std::stod(match[1].str());
    }
    return 0.0;
}

uint64_t SimpleJsonParser::extractUint64Field(const std::string& json, const std::string& field) {
    std::string pattern = "\"" + field + "\"\\s*:\\s*(\\d+)";
    std::regex re(pattern);
    std::smatch match;

    if (std::regex_search(json, match, re) && match.size() > 1) {
        return std::stoull(match[1].str());
    }
    return 0;
}

bool SimpleJsonParser::extractBoolField(const std::string& json, const std::string& field) {
    // 提取："field": true 或 "field": false
    std::string pattern = "\"" + field + "\"\\s*:\\s*(true|false)";
    std::regex re(pattern);
    std::smatch match;

    if (std::regex_search(json, match, re) && match.size() > 1) {
        return match[1].str() == "true";
    }
    return false;
}

std::string SimpleJsonParser::escapeJson(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            case '\0': oss << ""; break;  // 跳过空字符
            default:   
                // 只输出可打印字符，跳过其他控制字符
                if (c >= 32 && c <= 126) {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

// ============ 心跳请求解析 ============

application::HeartbeatRequestDTO SimpleJsonParser::parseHeartbeatRequest(const std::string& json) {
    application::HeartbeatRequestDTO dto;

    dto.api_version = extractIntField(json, "api_version");

    dto.data.box_id = extractIntField(json, "box_id");
    dto.data.slot_id = extractIntField(json, "slot_id");
    dto.data.cpu_id = extractIntField(json, "cpu_id");
    dto.data.srio_id = extractIntField(json, "srio_id");
    dto.data.host_ip = extractStringField(json, "host_ip");
    dto.data.hostname = extractStringField(json, "hostname");
    dto.data.service_port = extractUint16Field(json, "service_port");
    dto.data.box_type = extractStringField(json, "box_type");
    dto.data.board_type = extractStringField(json, "board_type");
    dto.data.cpu_type = extractStringField(json, "cpu_type");
    dto.data.os_type = extractStringField(json, "os_type");
    dto.data.resource_type = extractStringField(json, "resource_type");
    dto.data.cpu_arch = extractStringField(json, "cpu_arch");

    // 解析GPU数组（简化版：只提取index和name）
    // 实际应该用JSON库来解析数组
    // 暂时跳过GPU解析，避免正则表达式复杂性
    // TODO: 使用nlohmann/json库来正确解析JSON数组

    return dto;
}

// ============ 资源请求解析 ============

monitoring::application::ResourceReportRequestDTO monitoring::presentation::SimpleJsonParser::parseResourceRequest(const std::string& json) {
    application::ResourceReportRequestDTO dto;

    dto.api_version = extractIntField(json, "api_version");
    dto.data.host_ip = extractStringField(json, "host_ip");

    // CPU
    dto.data.resource.cpu.usage_percent = extractDoubleField(json, "usage_percent");
    dto.data.resource.cpu.load_avg_1m = extractDoubleField(json, "load_avg_1m");
    dto.data.resource.cpu.load_avg_5m = extractDoubleField(json, "load_avg_5m");
    dto.data.resource.cpu.load_avg_15m = extractDoubleField(json, "load_avg_15m");
    dto.data.resource.cpu.core_count = extractIntField(json, "core_count");
    dto.data.resource.cpu.core_allocated = extractIntField(json, "core_allocated");
    dto.data.resource.cpu.temperature = extractDoubleField(json, "temperature");
    dto.data.resource.cpu.voltage = extractDoubleField(json, "voltage");
    dto.data.resource.cpu.current = extractDoubleField(json, "current");
    dto.data.resource.cpu.power = extractDoubleField(json, "power");

    // Memory
    dto.data.resource.memory.total = extractUint64Field(json, "total");
    dto.data.resource.memory.used = extractUint64Field(json, "used");
    dto.data.resource.memory.free = extractUint64Field(json, "free");
    dto.data.resource.memory.usage_percent = extractDoubleField(json, "usage_percent");

    // GPU (简化：假设只有基本字段)
    dto.data.resource.gpu_allocated = extractIntField(json, "gpu_allocated");
    dto.data.resource.gpu_num = extractIntField(json, "gpu_num");

    // 注意：完整的网络、磁盘、GPU数组解析需要真正的JSON库
    // 这里只解析了基本字段作为演示

    return dto;
}

// ============ 响应序列化 ============

std::string monitoring::presentation::SimpleJsonParser::serializeHeartbeatResponse(const monitoring::application::HeartbeatResponseDTO& dto) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"code\": " << dto.code << ",\n";
    json << "  \"message\": \"" << escapeJson(dto.message) << "\"\n";
    json << "}\n";
    return json.str();
}

std::string monitoring::presentation::SimpleJsonParser::serializeResourceResponse(const monitoring::application::ResourceReportResponseDTO& dto) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"code\": " << dto.code << ",\n";
    json << "  \"message\": \"" << escapeJson(dto.message) << "\"\n";
    json << "}\n";
    return json.str();
}

// ============ 告警规则管理API相关方法实现 ============

monitoring::application::CreateAlertRuleRequestDTO monitoring::presentation::SimpleJsonParser::parseCreateAlertRuleRequest(const std::string& json) {
    monitoring::application::CreateAlertRuleRequestDTO dto;
    
    dto.ruleName = extractStringField(json, "ruleName");
    dto.metricName = extractStringField(json, "metricName");
    dto.threshold = extractDoubleField(json, "threshold");
    dto.operator_ = extractStringField(json, "operator");
    dto.durationSeconds = extractIntField(json, "durationSeconds");
    dto.severity = extractStringField(json, "severity");
    dto.isEnabled = extractBoolField(json, "isEnabled");
    dto.description = extractStringField(json, "description");
    
    return dto;
}

monitoring::application::UpdateAlertRuleRequestDTO monitoring::presentation::SimpleJsonParser::parseUpdateAlertRuleRequest(const std::string& json) {
    monitoring::application::UpdateAlertRuleRequestDTO dto;
    
    dto.ruleName = extractStringField(json, "ruleName");
    dto.metricName = extractStringField(json, "metricName");
    dto.threshold = extractDoubleField(json, "threshold");
    dto.operator_ = extractStringField(json, "operator");
    dto.durationSeconds = extractIntField(json, "durationSeconds");
    dto.severity = extractStringField(json, "severity");
    dto.isEnabled = extractBoolField(json, "isEnabled");
    dto.description = extractStringField(json, "description");
    
    return dto;
}

std::string monitoring::presentation::SimpleJsonParser::serializeAlertRuleDetailResponse(const monitoring::application::AlertRuleDetailResponseDTO& dto) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"code\": " << dto.code << ",\n";
    json << "  \"message\": \"" << escapeJson(dto.message) << "\",\n";
    json << "  \"rule\": {\n";
    json << "    \"ruleId\": " << dto.rule.ruleId << ",\n";
    json << "    \"ruleName\": \"" << escapeJson(dto.rule.ruleName) << "\",\n";
    json << "    \"metricName\": \"" << escapeJson(dto.rule.metricName) << "\",\n";
    json << "    \"threshold\": " << dto.rule.threshold << ",\n";
    json << "    \"operator\": \"" << escapeJson(dto.rule.operator_) << "\",\n";
    json << "    \"durationSeconds\": " << dto.rule.durationSeconds << ",\n";
    json << "    \"severity\": \"" << escapeJson(dto.rule.severity) << "\",\n";
    json << "    \"isEnabled\": " << (dto.rule.isEnabled ? "true" : "false") << ",\n";
    json << "    \"description\": \"" << escapeJson(dto.rule.description) << "\",\n";
    json << "    \"createdAt\": " << dto.rule.createdAt << ",\n";
    json << "    \"updatedAt\": " << dto.rule.updatedAt << "\n";
    json << "  }\n";
    json << "}\n";
    return json.str();
}

std::string monitoring::presentation::SimpleJsonParser::serializeAlertRuleListResponse(const monitoring::application::AlertRuleListResponseDTO& dto) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"code\": " << dto.code << ",\n";
    json << "  \"message\": \"" << escapeJson(dto.message) << "\",\n";
    json << "  \"total\": " << dto.total << ",\n";
    json << "  \"rules\": [\n";
    
    for (size_t i = 0; i < dto.rules.size(); ++i) {
        const auto& rule = dto.rules[i];
        json << "    {\n";
        json << "      \"ruleId\": " << rule.ruleId << ",\n";
        json << "      \"ruleName\": \"" << escapeJson(rule.ruleName) << "\",\n";
        json << "      \"metricName\": \"" << escapeJson(rule.metricName) << "\",\n";
        json << "      \"threshold\": " << rule.threshold << ",\n";
        json << "      \"operator\": \"" << escapeJson(rule.operator_) << "\",\n";
        json << "      \"durationSeconds\": " << rule.durationSeconds << ",\n";
        json << "      \"severity\": \"" << escapeJson(rule.severity) << "\",\n";
        json << "      \"isEnabled\": " << (rule.isEnabled ? "true" : "false") << ",\n";
        json << "      \"description\": \"" << escapeJson(rule.description) << "\",\n";
        json << "      \"createdAt\": " << rule.createdAt << ",\n";
        json << "      \"updatedAt\": " << rule.updatedAt << "\n";
        json << "    }";
        if (i < dto.rules.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "  ]\n";
    json << "}\n";
    return json.str();
}

std::string monitoring::presentation::SimpleJsonParser::serializeCommonResponse(const monitoring::application::CommonResponseDTO& dto) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"code\": " << dto.code << ",\n";
    json << "  \"message\": \"" << escapeJson(dto.message) << "\"\n";
    json << "}\n";
    return json.str();
}

// ============ 告警事件序列化方法实现 ============

std::string monitoring::presentation::SimpleJsonParser::serializeAlertEventDetailResponse(
    const application::AlertEventDetailResponseDTO& dto
) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":" << dto.code << ",";
    oss << "\"message\":\"" << escapeJson(dto.message) << "\",";
    oss << "\"event\":{";
    oss << "\"eventId\":" << dto.event.eventId << ",";
    oss << "\"ruleId\":" << dto.event.ruleId << ",";
    oss << "\"nodeId\":\"" << escapeJson(dto.event.nodeId) << "\",";
    oss << "\"status\":\"" << escapeJson(dto.event.status) << "\",";
    oss << "\"severity\":\"" << escapeJson(dto.event.severity) << "\",";
    oss << "\"startAt\":" << dto.event.startAt << ",";
    oss << "\"endAt\":" << dto.event.endAt << ",";
    oss << "\"triggeredValue\":" << dto.event.triggeredValue << ",";
    oss << "\"details\":\"" << escapeJson(dto.event.details) << "\",";
    oss << "\"acknowledgedBy\":\"" << escapeJson(dto.event.acknowledgedBy) << "\",";
    oss << "\"acknowledgedAt\":" << dto.event.acknowledgedAt;
    oss << "}";
    oss << "}";
    return oss.str();
}

std::string monitoring::presentation::SimpleJsonParser::serializeAlertEventListResponse(
    const application::AlertEventListResponseDTO& dto
) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":" << dto.code << ",";
    oss << "\"message\":\"" << escapeJson(dto.message) << "\",";
    oss << "\"total\":" << dto.total << ",";
    oss << "\"events\":[";
    
    for (size_t i = 0; i < dto.events.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& event = dto.events[i];
        oss << "{";
        oss << "\"eventId\":" << event.eventId << ",";
        oss << "\"ruleId\":" << event.ruleId << ",";
        oss << "\"nodeId\":\"" << escapeJson(event.nodeId) << "\",";
        oss << "\"status\":\"" << escapeJson(event.status) << "\",";
        oss << "\"severity\":\"" << escapeJson(event.severity) << "\",";
        oss << "\"startAt\":" << event.startAt << ",";
        oss << "\"endAt\":" << event.endAt << ",";
        oss << "\"triggeredValue\":" << event.triggeredValue << ",";
        oss << "\"details\":\"" << escapeJson(event.details) << "\",";
        oss << "\"acknowledgedBy\":\"" << escapeJson(event.acknowledgedBy) << "\",";
        oss << "\"acknowledgedAt\":" << event.acknowledgedAt;
        oss << "}";
    }
    
    oss << "]";
    oss << "}";
    return oss.str();
}

} // namespace monitoring::presentation
