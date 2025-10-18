#include "alert_expression_evaluation_service.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace monitoring::domain {

AlertEvaluationResult AlertExpressionEvaluationService::evaluate(
    const AlertExpression& expression,
    const MetricSnapshot& metrics,
    const std::string& nodeId
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
        bool conditionResult = evaluateCondition(condition, metrics, expression.tags);
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
    const std::map<std::string, std::string>& globalTags
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
            if (!matchTags(tagsToUse, metrics)) {
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
    const MetricSnapshot& metrics
) {
    // 目前简化实现：检查节点ID是否匹配
    // 在实际应用中，这里应该检查更多的标签信息
    for (const auto& tag : tags) {
        if (tag.first == "node_id") {
            if (metrics.getNodeId() != tag.second) {
                return false;
            }
        }
        // 可以添加更多标签匹配逻辑
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
