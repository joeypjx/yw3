#include "alert_expression_evaluation_service.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace monitoring::domain {

AlertEvaluationResult AlertExpressionEvaluationService::evaluate(
    const AlertExpression& expression,
    const MetricSnapshot& metrics,
    const ServerNode& node
) {
    AlertEvaluationResult result;
    result.triggered = false;
    result.triggeredValue = 0.0;
    result.reason = "";
    
    if (expression.conditions.empty()) {
        result.reason = "No conditions defined";
        return result;
    }
    
    std::vector<bool> conditionResults;
    std::vector<double> conditionValues;
    
    // 评估每个条件
    for (const auto& condition : expression.conditions) {
        bool conditionResult = evaluateCondition(condition, metrics, expression.tags, node);
        conditionResults.push_back(conditionResult);
        
        // 获取条件对应的指标值
        try {
            double value = metrics.extractMetricValue(condition.metric);
            conditionValues.push_back(value);
            
            if (conditionResult) {
                result.matchedConditions.push_back(
                    condition.metric + " " + condition.operator_ + " " + std::to_string(condition.threshold)
                );
            } else {
                result.failedConditions.push_back(
                    condition.metric + " " + condition.operator_ + " " + std::to_string(condition.threshold) + 
                    " (actual: " + std::to_string(value) + ")"
                );
            }
        } catch (const std::exception& e) {
            result.failedConditions.push_back(
                condition.metric + " (error: " + std::string(e.what()) + ")"
            );
            conditionValues.push_back(0.0);
        }
    }
    
    // 根据逻辑关系确定最终结果
    if (expression.logic == "AND") {
        result.triggered = std::all_of(conditionResults.begin(), conditionResults.end(), [](bool b) { return b; });
        if (result.triggered) {
            result.triggeredValue = *std::max_element(conditionValues.begin(), conditionValues.end());
            result.reason = "All conditions met (AND logic)";
        } else {
            result.reason = "Not all conditions met (AND logic)";
        }
    } else if (expression.logic == "OR") {
        result.triggered = std::any_of(conditionResults.begin(), conditionResults.end(), [](bool b) { return b; });
        if (result.triggered) {
            // 找到第一个触发的条件对应的值
            for (size_t i = 0; i < conditionResults.size(); ++i) {
                if (conditionResults[i]) {
                    result.triggeredValue = conditionValues[i];
                    break;
                }
            }
            result.reason = "At least one condition met (OR logic)";
        } else {
            result.reason = "No conditions met (OR logic)";
        }
    } else {
        result.reason = "Unknown logic operator: " + expression.logic;
    }
    
    return result;
}

bool AlertExpressionEvaluationService::evaluateCondition(
    const AlertCondition& condition,
    const MetricSnapshot& metrics,
    const std::map<std::string, std::string>& globalTags,
    const ServerNode& node
) {
    try {
        // 确定使用哪个标签集合：条件级标签优先，否则使用全局标签
        std::map<std::string, std::string> tagsToUse;
        if (!condition.tags.empty()) {
            tagsToUse = condition.tags;
        } else if (!globalTags.empty()) {
            tagsToUse = globalTags;
        }
        
        // 检查标签匹配（如果有标签的话）
        if (!tagsToUse.empty()) {
            if (!matchTags(tagsToUse, metrics, node)) {
                return false;
            }
        }
        
        // 获取指标值
        double value = metrics.extractMetricValue(condition.metric);
        
        // 比较值
        return compareValues(value, condition.threshold, condition.operator_);
        
    } catch (const std::exception& e) {
        std::cerr << "[AlertExpressionEvaluationService] Error evaluating condition: " << e.what() << std::endl;
        return false;
    }
}

bool AlertExpressionEvaluationService::matchTags(
    const std::map<std::string, std::string>& tags,
    const MetricSnapshot& metrics,
    const ServerNode& node
) {
    // 增强的标签匹配实现：支持多种标签类型
    for (const auto& tag : tags) {
        const std::string& key = tag.first;
        const std::string& expectedValue = tag.second;
        
        // 节点ID匹配
        if (key == "node_id") {
            if (metrics.getNodeId() != expectedValue) {
                return false;
            }
        }
        // 主机IP地址匹配
        else if (key == "host_ip") {
            if (node.getIpAddress() != expectedValue) {
                return false;
            }
        }
        // 主机名匹配
        else if (key == "hostname") {
            if (node.getHostname() != expectedValue) {
                return false;
            }
        }
        // 机箱ID匹配
        else if (key == "box_id") {
            try {
                int32_t expectedBoxId = std::stoi(expectedValue);
                if (node.getHardwareInfo().boxId != expectedBoxId) {
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "[AlertExpressionEvaluationService] Invalid box_id value: " << expectedValue << std::endl;
                return false;
            }
        }
        // 槽位ID匹配
        else if (key == "slot_id") {
            try {
                int32_t expectedSlotId = std::stoi(expectedValue);
                if (node.getHardwareInfo().slotId != expectedSlotId) {
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "[AlertExpressionEvaluationService] Invalid slot_id value: " << expectedValue << std::endl;
                return false;
            }
        }
        // CPU ID匹配
        else if (key == "cpu_id") {
            try {
                int32_t expectedCpuId = std::stoi(expectedValue);
                if (node.getHardwareInfo().cpuId != expectedCpuId) {
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "[AlertExpressionEvaluationService] Invalid cpu_id value: " << expectedValue << std::endl;
                return false;
            }
        }
        // SRIO ID匹配
        else if (key == "srio_id") {
            try {
                int32_t expectedSrioId = std::stoi(expectedValue);
                if (node.getHardwareInfo().srioId != expectedSrioId) {
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "[AlertExpressionEvaluationService] Invalid srio_id value: " << expectedValue << std::endl;
                return false;
            }
        }
        // 服务端口匹配
        else if (key == "service_port") {
            try {
                uint16_t expectedPort = static_cast<uint16_t>(std::stoi(expectedValue));
                if (node.getHardwareInfo().servicePort != expectedPort) {
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "[AlertExpressionEvaluationService] Invalid service_port value: " << expectedValue << std::endl;
                return false;
            }
        }
        // 机箱类型匹配
        else if (key == "box_type") {
            if (node.getHardwareInfo().boxType != expectedValue) {
                return false;
            }
        }
        // 板卡类型匹配
        else if (key == "board_type") {
            if (node.getHardwareInfo().boardType != expectedValue) {
                return false;
            }
        }
        // CPU类型匹配
        else if (key == "cpu_type") {
            if (node.getHardwareInfo().cpuType != expectedValue) {
                return false;
            }
        }
        // 操作系统类型匹配
        else if (key == "os_type") {
            if (node.getHardwareInfo().osType != expectedValue) {
                return false;
            }
        }
        // 资源类型匹配
        else if (key == "resource_type") {
            if (node.getHardwareInfo().resourceType != expectedValue) {
                return false;
            }
        }
        // CPU架构匹配
        else if (key == "cpu_arch") {
            if (node.getHardwareInfo().cpuArch != expectedValue) {
                return false;
            }
        }
        // GPU数量匹配
        else if (key == "gpu_count") {
            try {
                int32_t expectedGpuCount = std::stoi(expectedValue);
                if (static_cast<int32_t>(node.getHardwareInfo().gpus.size()) != expectedGpuCount) {
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "[AlertExpressionEvaluationService] Invalid gpu_count value: " << expectedValue << std::endl;
                return false;
            }
        }
        // 未知标签类型
        else {
            std::cerr << "[AlertExpressionEvaluationService] Unknown tag type: " << key << std::endl;
            return false;
        }
    }
    
    return true;
}

bool AlertExpressionEvaluationService::compareValues(double value, double threshold, const std::string& op) {
    if (op == ">") {
        return value > threshold;
    } else if (op == "<") {
        return value < threshold;
    } else if (op == ">=") {
        return value >= threshold;
    } else if (op == "<=") {
        return value <= threshold;
    } else if (op == "==") {
        return std::abs(value - threshold) < 1e-9; // 浮点数比较
    } else if (op == "!=") {
        return std::abs(value - threshold) >= 1e-9;
    } else {
        std::cerr << "[AlertExpressionEvaluationService] Unknown operator: " << op << std::endl;
        return false;
    }
}

} // namespace monitoring::domain
