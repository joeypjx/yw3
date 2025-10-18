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
        dto.api_version = j["api_version"].get<int32_t>();
        
        // 解析data字段
        json data = j["data"];
        dto.data.box_id = data["box_id"].get<int32_t>();
        dto.data.slot_id = data["slot_id"].get<int32_t>();
        dto.data.cpu_id = data["cpu_id"].get<int32_t>();
        dto.data.srio_id = data["srio_id"].get<int32_t>();
        dto.data.host_ip = data["host_ip"].get<std::string>();
        dto.data.hostname = data["hostname"].get<std::string>();
        dto.data.service_port = data["service_port"].get<uint16_t>();
        dto.data.box_type = data["box_type"].get<std::string>();
        dto.data.board_type = data["board_type"].get<std::string>();
        dto.data.cpu_type = data["cpu_type"].get<std::string>();
        dto.data.os_type = data["os_type"].get<std::string>();
        dto.data.resource_type = data["resource_type"].get<std::string>();
        dto.data.cpu_arch = data["cpu_arch"].get<std::string>();
        
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
        dto.api_version = j["api_version"].get<int32_t>();
        
        // 解析data字段
        json data = j["data"];
        dto.data.host_ip = data["host_ip"].get<std::string>();
        
        // 解析resource字段
        json resource = data["resource"];
        
        // CPU信息
        json cpu = resource["cpu"];
        dto.data.resource.cpu.usage_percent = cpu["usage_percent"].get<double>();
        dto.data.resource.cpu.load_avg_1m = cpu["load_avg_1m"].get<double>();
        dto.data.resource.cpu.load_avg_5m = cpu["load_avg_5m"].get<double>();
        dto.data.resource.cpu.load_avg_15m = cpu["load_avg_15m"].get<double>();
        dto.data.resource.cpu.core_count = cpu["core_count"].get<int32_t>();
        dto.data.resource.cpu.core_allocated = cpu["core_allocated"].get<int32_t>();
        dto.data.resource.cpu.temperature = cpu["temperature"].get<double>();
        dto.data.resource.cpu.voltage = cpu["voltage"].get<double>();
        dto.data.resource.cpu.current = cpu["current"].get<double>();
        dto.data.resource.cpu.power = cpu["power"].get<double>();
        
        // 内存信息
        json memory = resource["memory"];
        dto.data.resource.memory.total = memory["total"].get<uint64_t>();
        dto.data.resource.memory.used = memory["used"].get<uint64_t>();
        dto.data.resource.memory.free = memory["free"].get<uint64_t>();
        dto.data.resource.memory.usage_percent = memory["usage_percent"].get<double>();
        
        // 磁盘信息
        json disks = resource["disk"];
        for (const auto& disk : disks) {
            application::DiskPartitionDTO diskInfo;
            diskInfo.device = disk["device"].get<std::string>();
            diskInfo.mount_point = disk["mount_point"].get<std::string>();
            diskInfo.total = disk["total"].get<uint64_t>();
            diskInfo.used = disk["used"].get<uint64_t>();
            diskInfo.free = disk["free"].get<uint64_t>();
            diskInfo.usage_percent = disk["usage_percent"].get<double>();
            dto.data.resource.disk.push_back(diskInfo);
        }
        
        // 网络信息
        json networks = resource["network"];
        for (const auto& network : networks) {
            application::NetworkInterfaceDTO networkInfo;
            networkInfo.interface = network["interface"].get<std::string>();
            networkInfo.rx_bytes = network["rx_bytes"].get<uint64_t>();
            networkInfo.tx_bytes = network["tx_bytes"].get<uint64_t>();
            networkInfo.rx_packets = network["rx_packets"].get<uint64_t>();
            networkInfo.tx_packets = network["tx_packets"].get<uint64_t>();
            networkInfo.rx_errors = network["rx_errors"].get<uint64_t>();
            networkInfo.tx_errors = network["tx_errors"].get<uint64_t>();
            networkInfo.rx_rate = network["rx_rate"].get<uint64_t>();
            networkInfo.tx_rate = network["tx_rate"].get<uint64_t>();
            dto.data.resource.network.push_back(networkInfo);
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
        dto.alert_name = j["alert_name"].get<std::string>();
        dto.alert_type = j.value("alert_type", "");
        dto.description = j.value("description", "");
        dto.enabled = j.value("enabled", true);
        dto.severity = j["severity"].get<std::string>();
        dto.summary = j.value("summary", "");
        dto.for_ = j["for"].get<std::string>();
         
        // 解析表达式
        if (j.contains("expression") && j["expression"].is_object()) {
            json expression = j["expression"];
            
            // 解析表达式级字段
            dto.expression.metric = expression["metric"].get<std::string>();
            dto.expression.stable = expression.value("stable", "");
            
            // 解析条件数组
            if (expression.contains("conditions") && expression["conditions"].is_array()) {
                for (const auto& conditionJson : expression["conditions"]) {
                    application::AlertCondition condition;
                    condition.operator_ = conditionJson["operator"].get<std::string>();
                    condition.threshold = conditionJson["threshold"].get<double>();
                    
                    dto.expression.conditions.push_back(condition);
                }
            }
            
            // 解析表达式级全局标签（可选）
            if (expression.contains("tags") && expression["tags"].is_array()) {
                json globalTags = expression["tags"];
                for (const auto& tagJson : globalTags) {
                    application::TagPair tagPair;
                    // 假设标签是对象格式 {"key": "value"}
                    for (auto& tag : tagJson.items()) {
                        tagPair.key = tag.key();
                        tagPair.value = tag.value().get<std::string>();
                        dto.expression.tags.push_back(tagPair);
                    }
                }
            }
        }

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
        dto.alert_name = j.value("alert_name", "");
        dto.alert_type = j.value("alert_type", "");
        dto.description = j.value("description", "");
        dto.enabled = j.value("enabled", true);
        dto.severity = j.value("severity", "");
        dto.summary = j.value("summary", "");
        dto.for_ = j.value("for", "");
        
        // 解析表达式
        if (j.contains("expression") && j["expression"].is_object()) {
            json expression = j["expression"];
            
            // 解析表达式级字段
            dto.expression.metric = expression.value("metric", "");
            dto.expression.stable = expression.value("stable", "");
            
            // 解析条件数组
            if (expression.contains("conditions") && expression["conditions"].is_array()) {
                for (const auto& conditionJson : expression["conditions"]) {
                    application::AlertCondition condition;
                    condition.operator_ = conditionJson["operator"].get<std::string>();
                    condition.threshold = conditionJson["threshold"].get<double>();
                    
                    dto.expression.conditions.push_back(condition);
                }
            }
            
            // 解析表达式级全局标签（可选）
            if (expression.contains("tags") && expression["tags"].is_array()) {
                json globalTags = expression["tags"];
                for (const auto& tagJson : globalTags) {
                    application::TagPair tagPair;
                    // 假设标签是对象格式 {"key": "value"}
                    for (auto& tag : tagJson.items()) {
                        tagPair.key = tag.key();
                        tagPair.value = tag.value().get<std::string>();
                        dto.expression.tags.push_back(tagPair);
                    }
                }
            }
        }

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
    
    return j.dump();
}

std::string SimpleJsonParser::serializeResourceResponse(const application::ResourceReportResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    
    return j.dump();
}

std::string SimpleJsonParser::serializeAlertRuleDetailResponse(const application::AlertRuleDetailResponseDTO& dto) {
    json j;
    j["code"] = dto.code;
    j["message"] = dto.message;
    
    json rule;
    rule["id"] = dto.rule.id;
    rule["alert_name"] = dto.rule.alert_name;
    rule["alert_type"] = dto.rule.alert_type;
    rule["created_at"] = dto.rule.created_at;
    rule["description"] = dto.rule.description;
    rule["enabled"] = dto.rule.enabled;
    rule["severity"] = dto.rule.severity;
    rule["summary"] = dto.rule.summary;
    rule["updated_at"] = dto.rule.updated_at;
    rule["for"] = dto.rule.for_;
    
    // 序列化表达式
    json expression;
    expression["metric"] = dto.rule.expression.metric;
    expression["stable"] = dto.rule.expression.stable;
    
    // 序列化条件数组
    json conditions = json::array();
    for (const auto& condition : dto.rule.expression.conditions) {
        json conditionJson;
        conditionJson["operator"] = condition.operator_;
        conditionJson["threshold"] = condition.threshold;
        
        conditions.push_back(conditionJson);
    }
    expression["conditions"] = conditions;
    
    // 序列化全局标签
    if (!dto.rule.expression.tags.empty()) {
        json globalTags = json::array();
        for (const auto& tag : dto.rule.expression.tags) {
            json tagJson;
            tagJson[tag.key] = tag.value;
            globalTags.push_back(tagJson);
        }
        expression["tags"] = globalTags;
    }
    
    rule["expression"] = expression;
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
        ruleJson["id"] = rule.id;
        ruleJson["alert_name"] = rule.alert_name;
        ruleJson["alert_type"] = rule.alert_type;
        ruleJson["created_at"] = rule.created_at;
        ruleJson["description"] = rule.description;
        ruleJson["enabled"] = rule.enabled;
        ruleJson["severity"] = rule.severity;
        ruleJson["summary"] = rule.summary;
        ruleJson["updated_at"] = rule.updated_at;
        ruleJson["for"] = rule.for_;
        
        // 序列化表达式
        json expression;
        expression["metric"] = rule.expression.metric;
        expression["stable"] = rule.expression.stable;
        
        // 序列化条件数组
        json conditions = json::array();
        for (const auto& condition : rule.expression.conditions) {
            json conditionJson;
            conditionJson["operator"] = condition.operator_;
            conditionJson["threshold"] = condition.threshold;
            
            conditions.push_back(conditionJson);
        }
        expression["conditions"] = conditions;
        
        // 序列化全局标签
        if (!rule.expression.tags.empty()) {
            json globalTags = json::array();
            for (const auto& tag : rule.expression.tags) {
                json tagJson;
                tagJson[tag.key] = tag.value;
                globalTags.push_back(tagJson);
            }
            expression["tags"] = globalTags;
        }
        
        ruleJson["expression"] = expression;
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
    
    // 注释
    json annotations;
    annotations["description"] = dto.event.annotations.description;
    annotations["summary"] = dto.event.annotations.summary;
    event["annotations"] = annotations;
    
    // 基本信息
    event["created_at"] = dto.event.created_at;
    event["ends_at"] = dto.event.ends_at;
    event["fingerprint"] = dto.event.fingerprint;
    event["id"] = dto.event.id;
    event["starts_at"] = dto.event.starts_at;
    event["status"] = dto.event.status;
    event["updated_at"] = dto.event.updated_at;
    
    // 标签
    json labels;
    labels["alert_type"] = dto.event.labels.alert_type;
    labels["alertname"] = dto.event.labels.alertname;
    labels["host_ip"] = dto.event.labels.host_ip;
    labels["metrics"] = dto.event.labels.metrics;
    labels["severity"] = dto.event.labels.severity;
    labels["value"] = dto.event.labels.value;
    event["labels"] = labels;
    
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
        
        // 注释
        json annotations;
        annotations["description"] = event.annotations.description;
        annotations["summary"] = event.annotations.summary;
        eventJson["annotations"] = annotations;
        
        // 基本信息
        eventJson["created_at"] = event.created_at;
        eventJson["ends_at"] = event.ends_at;
        eventJson["fingerprint"] = event.fingerprint;
        eventJson["id"] = event.id;
        eventJson["starts_at"] = event.starts_at;
        eventJson["status"] = event.status;
        eventJson["updated_at"] = event.updated_at;
        
        // 标签
        json labels;
        labels["alert_type"] = event.labels.alert_type;
        labels["alertname"] = event.labels.alertname;
        labels["host_ip"] = event.labels.host_ip;
        labels["metrics"] = event.labels.metrics;
        labels["severity"] = event.labels.severity;
        labels["value"] = event.labels.value;
        eventJson["labels"] = labels;
        
        events.push_back(eventJson);
    }
    
    j["events"] = events;
    
    return j.dump();
}

} // namespace monitoring::presentation
