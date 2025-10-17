#ifndef APPLICATION_SERVICES_ALERTING_ORCHESTRATOR_SERVICE_HPP
#define APPLICATION_SERVICES_ALERTING_ORCHESTRATOR_SERVICE_HPP

#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include "../../domain/repositories/i_server_node_repository.hpp"
#include "../../domain/repositories/i_metric_repository.hpp"
#include "../../domain/repositories/i_alert_rule_repository.hpp"
#include "../../domain/repositories/i_alert_event_repository.hpp"
#include "../../domain/services/i_notification_service.hpp"

namespace monitoring::application {

/**
 * AlertingOrchestratorService - 告警编排服务
 *
 * 职责：
 * 1. 定时执行告警检查
 * 2. 调用AlertRule::evaluate()进行决策
 * 3. 管理AlertEvent的生命周期（创建、解决）
 * 4. 发送通知
 *
 * 这是一个应用服务，负责告警流程的编排
 */
class AlertingOrchestratorService {
public:
    /**
     * 构造函数 - 依赖注入
     */
    AlertingOrchestratorService(
        domain::IServerNodeRepository* nodeRepository,
        domain::IMetricRepository* metricRepository,
        domain::IAlertRuleRepository* alertRuleRepository,
        domain::IAlertEventRepository* alertEventRepository,
        domain::INotificationService* notificationService,
        int32_t checkIntervalSeconds = 60
    );

    /**
     * 析构函数
     */
    ~AlertingOrchestratorService();

    /**
     * 启动告警检查服务
     */
    void start();

    /**
     * 停止告警检查服务
     */
    void stop();

    /**
     * 执行一次告警检查（手动触发）
     */
    void runChecks();

    /**
     * 检查服务是否正在运行
     */
    bool isRunning() const { return running_.load(); }

    /**
     * 获取检查间隔（秒）
     */
    int32_t getCheckInterval() const { return checkIntervalSeconds_; }

    /**
     * 设置检查间隔（秒）
     */
    void setCheckInterval(int32_t seconds) { checkIntervalSeconds_ = seconds; }

private:
    // 依赖注入的仓储和服务
    domain::IServerNodeRepository* nodeRepository_;
    domain::IMetricRepository* metricRepository_;
    domain::IAlertRuleRepository* alertRuleRepository_;
    domain::IAlertEventRepository* alertEventRepository_;
    domain::INotificationService* notificationService_;

    // 配置参数
    int32_t checkIntervalSeconds_;  // 检查间隔（秒）

    // 线程控制
    std::atomic<bool> running_;
    std::atomic<bool> shouldStop_;
    std::unique_ptr<std::thread> workerThread_;

    /**
     * 工作线程主循环
     */
    void workerLoop();

    /**
     * 处理单个节点的告警检查
     */
    void checkNodeAlerts(const domain::ServerNode& node);

    /**
     * 处理单个规则的告警检查
     */
    void checkRuleForNode(
        const domain::AlertRule& rule,
        const domain::ServerNode& node
    );

    /**
     * 创建新的告警事件
     */
    void createAlertEvent(
        const domain::AlertRule& rule,
        const domain::ServerNode& node,
        double triggeredValue,
        const std::string& details
    );

    /**
     * 解决现有告警事件
     */
    void resolveAlertEvent(
        const domain::AlertEvent& event,
        double currentValue,
        const std::string& details
    );
};

} // namespace monitoring::application

#endif // APPLICATION_SERVICES_ALERTING_ORCHESTRATOR_SERVICE_HPP
