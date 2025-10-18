#include "simple_json_parser.hpp"
#include <stdexcept>
#include <iostream>

namespace monitoring::presentation {

using json = nlohmann::json;

// ============ 解析方法实现 ============

application::HeartbeatRequestDTO SimpleJsonParser::parseHeartbeatRequest(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        application::HeartbeatRequestDTO dto;
        dto.apiVersion = j["api_version"].get<int32_t>();
        
        // 解析data字段
        json data = j["data"];
        dto.data.boxId = data["box_id"].get<int32_t>();
        dto.data.slotId = data["slot_id"].get<int32_t>();
        dto.data.cpuId = data["cpu_id"].get<int32_t>();
        dto.data.hostIp = data["host_ip"].get<std::string>();
        dto.data.hostname = data["hostname"].get<std::string>();
        dto.data.osVersion = data["os_version"].get<std::string>();
        dto.data.kernelVersion = data["kernel_version"].get<std::string>();
        dto.data.uptime = data["uptime"].get<uint64_t>();
        
        return dto;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing heartbeat request: " << e.what() << std::endl;
        throw std::runtime_error("Invalid JSON format for heartbeat request");
    }
}

application::ResourceReportRequestDTO SimpleJsonParser::parseResourceRequest(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        application::ResourceReportRequestDTO dto;
        dto.apiVersion = j["api_version"].get<int32_t>();
        
        // 解析data字段
        json data = j["data"];
        dto.data.hostIp = data["host_ip"].get<std::string>();
        
        // 解析resource字段
        json resource = data["resource"];
        
        // CPU信息
        json cpu = resource["cpu"];
        dto.data.resource.cpu.usagePercent = cpu["usage_percent"].get<double>();
        dto.data.resource.cpu.frequency = cpu["frequency"].get<uint64_t>();
        dto.data.resource.cpu.cores = cpu["cores"].get<int32_t>();
        
        // 内存信息
        json memory = resource["memory"];
        dto.data.resource.memory.total = memory["total"].get<uint64_t>();
        dto.data.resource.memory.used = memory["used"].get<uint64_t>();
        dto.data.resource.memory.free = memory["free"].get<uint64_t>();
        dto.data.resource.memory.usagePercent = memory["usage_percent"].get<double>();
        
        // 磁盘信息
        json disks = resource["disks"];
        for (const auto& disk : disks) {
            application::DiskInfo diskInfo;
            diskInfo.device = disk["device"].get<std::string>();
            diskInfo.mountPoint = disk["mount_point"].get<std::string>();
            diskInfo.total = disk["total"].get<uint64_t>();
            diskInfo.used = disk["used"].get<uint64_t>();
            diskInfo.free = disk["free"].get<uint64_t>();
            diskInfo.usagePercent = disk["usage_percent"].get<double>();
            dto.data.resource.disks.push_back(diskInfo);
        }
        
        // 网络信息
        json networks = resource["networks"];
        for (const auto& network : networks) {
            application::NetworkInfo networkInfo;
            networkInfo.interface = network["interface"].get<std::string>();
            networkInfo.rxBytes = network["rx_bytes"].get<uint64_t>();
            networkInfo.txBytes = network["tx_bytes"].get<uint64_t>();
            networkInfo.rxRate = network["rx_rate"].get<uint64_t>();
            networkInfo.txRate = network["tx_rate"].get<uint64_t>();
            dto.data.resource.networks.push_back(networkInfo);
        }
        
        return dto;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing resource request: " << e.what() << std::endl;
        throw std::runtime_error("Invalid JSON format for resource request");
    }
}

application::CreateAlertRuleRequestDTO SimpleJsonParser::parseCreateAlertRuleRequest(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        application::CreateAlertRuleRequestDTO dto;
        dto.ruleName = j["ruleName"].get<std::string>();
        dto.metricName = j["metricName"].get<std::string>();
        dto.threshold = j["threshold"].get<double>();
        dto.operator_ = j["operator"].get<std::string>();
        dto.durationSeconds = j["durationSeconds"].get<int32_t>();
        dto.severity = j["severity"].get<std::string>();
        dto.isEnabled = j["isEnabled"].get<bool>();
        dto.description = j["description"].get<std::string>();
        
        return dto;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing create alert rule request: " << e.what() << std::endl;
        throw std::runtime_error("Invalid JSON format for create alert rule request");
    }
}

application::UpdateAlertRuleRequestDTO SimpleJsonParser::parseUpdateAlertRuleRequest(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        application::UpdateAlertRuleRequestDTO dto;
        dto.ruleName = j.value("ruleName", "");
        dto.metricName = j.value("metricName", "");
        dto.threshold = j.value("threshold", 0.0);
        dto.operator_ = j.value("operator", "");
        dto.durationSeconds = j.value("durationSeconds", 0);
        dto.severity = j.value("severity", "");
        dto.isEnabled = j.value("isEnabled", true);
        dto.description = j.value("description", "");
        
        return dto;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing update alert rule request: " << e.what() << std::endl;
        throw std::runtime_error("Invalid JSON format for update alert rule request");
    }
}

// ============ 序列化方法实现 ============

std::string SimpleJsonParser::serializeHeartbeatResponse(const application::HeartbeatResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    j["timestamp"] = dto.timestamp;
    
    return j.dump();
}

std::string SimpleJsonParser::serializeResourceResponse(const application::ResourceReportResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    j["timestamp"] = dto.timestamp;
    
    return j.dump();
}

std::string SimpleJsonParser::serializeAlertRuleDetailResponse(const application::AlertRuleDetailResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    
    json rule;
    rule["ruleId"] = dto.rule.ruleId;
    rule["ruleName"] = dto.rule.ruleName;
    rule["metricName"] = dto.rule.metricName;
    rule["threshold"] = dto.rule.threshold;
    rule["operator"] = dto.rule.operator_;
    rule["durationSeconds"] = dto.rule.durationSeconds;
    rule["severity"] = dto.rule.severity;
    rule["isEnabled"] = dto.rule.isEnabled;
    rule["description"] = dto.rule.description;
    rule["createdAt"] = dto.rule.createdAt;
    rule["updatedAt"] = dto.rule.updatedAt;
    
    j["rule"] = rule;
    
    return j.dump();
}

std::string SimpleJsonParser::serializeAlertRuleListResponse(const application::AlertRuleListResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    j["total"] = dto.total;
    
    json rules = json::array();
    for (const auto& rule : dto.rules) {
        json ruleJson;
        ruleJson["ruleId"] = rule.ruleId;
        ruleJson["ruleName"] = rule.ruleName;
        ruleJson["metricName"] = rule.metricName;
        ruleJson["threshold"] = rule.threshold;
        ruleJson["operator"] = rule.operator_;
        ruleJson["durationSeconds"] = rule.durationSeconds;
        ruleJson["severity"] = rule.severity;
        ruleJson["isEnabled"] = rule.isEnabled;
        ruleJson["description"] = rule.description;
        ruleJson["createdAt"] = rule.createdAt;
        ruleJson["updatedAt"] = rule.updatedAt;
        
        rules.push_back(ruleJson);
    }
    
    j["rules"] = rules;
    
    return j.dump();
}

std::string SimpleJsonParser::serializeCommonResponse(const application::CommonResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    
    return j.dump();
}

std::string SimpleJsonParser::serializeAlertEventDetailResponse(const application::AlertEventDetailResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    
    json event;
    event["eventId"] = dto.event.eventId;
    event["ruleId"] = dto.event.ruleId;
    event["nodeId"] = dto.event.nodeId;
    event["status"] = dto.event.status;
    event["severity"] = dto.event.severity;
    event["startAt"] = dto.event.startAt;
    event["endAt"] = dto.event.endAt;
    event["triggeredValue"] = dto.event.triggeredValue;
    event["details"] = dto.event.details;
    event["acknowledgedBy"] = dto.event.acknowledgedBy;
    event["acknowledgedAt"] = dto.event.acknowledgedAt;
    
    j["event"] = event;
    
    return j.dump();
}

std::string SimpleJsonParser::serializeAlertEventListResponse(const application::AlertEventListResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    j["total"] = dto.total;
    
    json events = json::array();
    for (const auto& event : dto.events) {
        json eventJson;
        eventJson["eventId"] = event.eventId;
        eventJson["ruleId"] = event.ruleId;
        eventJson["nodeId"] = event.nodeId;
        eventJson["status"] = event.status;
        eventJson["severity"] = event.severity;
        eventJson["startAt"] = event.startAt;
        eventJson["endAt"] = event.endAt;
        eventJson["triggeredValue"] = event.triggeredValue;
        eventJson["details"] = event.details;
        eventJson["acknowledgedBy"] = event.acknowledgedBy;
        eventJson["acknowledgedAt"] = event.acknowledgedAt;
        
        events.push_back(eventJson);
    }
    
    j["events"] = events;
    
    return j.dump();
}

} // namespace monitoring::presentation
