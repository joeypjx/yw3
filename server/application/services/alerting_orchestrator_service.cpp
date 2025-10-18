#include "alerting_orchestrator_service.hpp"
#include "../../domain/entities/alert_rule.hpp"
#include "../../domain/entities/alert_event.hpp"
#include <iostream>
#include <sstream>

namespace monitoring::application {

AlertingOrchestratorService::AlertingOrchestratorService(
    domain::IServerNodeRepository* nodeRepository,
    domain::IMetricRepository* metricRepository,
    domain::IAlertRuleRepository* alertRuleRepository,
    domain::IAlertEventRepository* alertEventRepository,
    domain::INotificationService* notificationService,
    int32_t checkIntervalSeconds
)
    : nodeRepository_(nodeRepository)
    , metricRepository_(metricRepository)
    , alertRuleRepository_(alertRuleRepository)
    , alertEventRepository_(alertEventRepository)
    , notificationService_(notificationService)
    , checkIntervalSeconds_(checkIntervalSeconds)
    , running_(false)
    , shouldStop_(false)
{
}

AlertingOrchestratorService::~AlertingOrchestratorService() {
    stop();
}

void AlertingOrchestratorService::start() {
    if (running_.load()) {
        std::cout << "[AlertingOrchestrator] Already running, skipping start" << std::endl;
        return; // 已经在运行
    }

    std::cout << "[AlertingOrchestrator] Starting AlertingOrchestratorService..." << std::endl;
    shouldStop_.store(false);
    running_.store(true);
    
    workerThread_ = std::make_unique<std::thread>(&AlertingOrchestratorService::workerLoop, this);
    
    std::cout << "[AlertingOrchestrator] Started with interval " << checkIntervalSeconds_ << "s" << std::endl;
}

void AlertingOrchestratorService::stop() {
    if (!running_.load()) {
        return; // 已经停止
    }

    std::cout << "[AlertingOrchestrator] Stopping..." << std::endl;
    shouldStop_.store(true);
    
    if (workerThread_ && workerThread_->joinable()) {
        try {
            workerThread_->join();
        } catch (const std::exception& e) {
            std::cerr << "[AlertingOrchestrator] Error joining thread: " << e.what() << std::endl;
        }
    }
    
    running_.store(false);
    std::cout << "[AlertingOrchestrator] Stopped" << std::endl;
}

void AlertingOrchestratorService::runChecks() {
    try {
        // 1. 获取所有启用的告警规则
        auto activeRules = alertRuleRepository_->findAllActive();
        if (activeRules.empty()) {
            return; // 没有启用的规则
        }

        // 2. 获取所有节点
        auto allNodes = nodeRepository_->findAll();
        if (allNodes.empty()) {
            return; // 没有节点
        }

        std::cout << "[AlertingOrchestrator] Running checks for " << allNodes.size() 
                  << " nodes with " << activeRules.size() << " rules" << std::endl;

        // 3. 对每个节点执行告警检查
        for (const auto& node : allNodes) {
            checkNodeAlerts(node);
        }

    } catch (const std::exception& e) {
        std::cerr << "[AlertingOrchestrator] Error during checks: " << e.what() << std::endl;
    }
}

void AlertingOrchestratorService::workerLoop() {
    std::cout << "[AlertingOrchestrator] Worker loop started" << std::endl;
    while (!shouldStop_.load()) {
        std::cout << "[AlertingOrchestrator] Running checks..." << std::endl;
        runChecks();
        
        // 等待指定间隔
        std::cout << "[AlertingOrchestrator] Waiting " << checkIntervalSeconds_ << " seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(checkIntervalSeconds_));
    }
    std::cout << "[AlertingOrchestrator] Worker loop stopped" << std::endl;
}

void AlertingOrchestratorService::checkNodeAlerts(const domain::ServerNode& node) {
    try {
        // 获取该节点的所有启用的告警规则
        auto activeRules = alertRuleRepository_->findAllActive();
        
        for (const auto& rule : activeRules) {
            checkRuleForNode(rule, node);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[AlertingOrchestrator] Error checking node " << node.getNodeId() 
                  << ": " << e.what() << std::endl;
    }
}

void AlertingOrchestratorService::checkRuleForNode(
    const domain::AlertRule& rule,
    const domain::ServerNode& node
) {
    try {
        // 1. 获取该节点最近的数据（覆盖规则所需的时间窗口）
        auto recentMetrics = metricRepository_->findRecent(node.getNodeId(), rule.getDuration());
        
        // 2. 调用规则的评估方法
        std::cout << "[AlertingOrchestrator] Evaluating rule " << rule.getRuleName() 
                  << " for node " << node.getNodeId() << " with " << recentMetrics.size() 
                  << " metrics" << std::endl;
        auto result = rule.evaluate(node, recentMetrics);
        std::cout << "[AlertingOrchestrator] Evaluation result: " << static_cast<int>(result.status) 
                  << " (value: " << result.triggeredValue << ")" << std::endl;
        
        // 3. 检查是否已存在活跃的告警
        auto existingAlert = alertEventRepository_->findActiveEvent(rule.getRuleId(), node.getNodeId());
        
        // 4. 根据评估结果处理告警
        if (result.status == domain::EvaluationStatus::TRIGGER) {
            if (!existingAlert) {
                // 创建新告警
                createAlertEvent(rule, node, result.triggeredValue, result.reason);
            }
            // 如果已存在，则什么都不做，避免重复告警
        } 
        else if (result.status == domain::EvaluationStatus::RESOLVED) {
            if (existingAlert) {
                // 解决现有告警
                resolveAlertEvent(*existingAlert, result.triggeredValue, result.reason);
            }
        }
        // OK状态：什么都不做
        
    } catch (const std::exception& e) {
        std::cerr << "[AlertingOrchestrator] Error checking rule " << rule.getRuleId() 
                  << " for node " << node.getNodeId() << ": " << e.what() << std::endl;
    }
}

void AlertingOrchestratorService::createAlertEvent(
    const domain::AlertRule& rule,
    const domain::ServerNode& node,
    double triggeredValue,
    const std::string& details
) {
    try {
        // 生成事件ID（简单实现，生产环境应该使用更复杂的ID生成策略）
        static int64_t nextEventId = 1;
        int64_t eventId = nextEventId++;
        
        // 创建告警事件
        auto event = domain::AlertEvent::create(eventId, rule, node, triggeredValue, details);
        
        // 保存到仓储
        alertEventRepository_->save(event);
        
        // 发送通知
        notificationService_->sendAlertTriggered(event);
        
        std::cout << "[AlertingOrchestrator] Created alert event " << eventId 
                  << " for rule " << rule.getRuleId() << " on node " << node.getNodeId() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[AlertingOrchestrator] Error creating alert event: " << e.what() << std::endl;
    }
}

void AlertingOrchestratorService::resolveAlertEvent(
    const domain::AlertEvent& event,
    double currentValue,
    const std::string& details
) {
    try {
        // 解决告警事件
        auto resolvedEvent = event;
        resolvedEvent.resolve();
        
        // 保存到仓储
        alertEventRepository_->save(resolvedEvent);
        
        // 发送解决通知
        notificationService_->sendAlertResolved(resolvedEvent);
        
        std::cout << "[AlertingOrchestrator] Resolved alert event " << event.getEventId() 
                  << " for rule " << event.getRuleId() << " on node " << event.getNodeId() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[AlertingOrchestrator] Error resolving alert event: " << e.what() << std::endl;
    }
}

} // namespace monitoring::application
