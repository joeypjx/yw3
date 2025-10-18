#include "presentation/controllers/http_server.hpp"
#include "application/services/ingestion_service.hpp"
#include "application/services/alerting_orchestrator_service.hpp"
#include "application/services/alert_rule_management_service.hpp"
#include "application/services/alert_event_management_service.hpp"
#include "application/services/query_service.hpp"
#include "infrastructure/repositories/mock_node_repository.hpp"
#include "infrastructure/repositories/mock_metric_repository.hpp"
#include "infrastructure/repositories/mock_alert_rule_repository.hpp"
#include "infrastructure/repositories/mock_alert_event_repository.hpp"
#include "infrastructure/notifiers/mock_notification_service.hpp"
#include "domain/services/alert_expression_evaluation_service.hpp"
#include <iostream>
#include <csignal>
#include <memory>

// 全局服务器指针，用于信号处理
monitoring::presentation::HttpServer* g_server = nullptr;
monitoring::application::AlertingOrchestratorService* g_alerting_service = nullptr;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down server..." << std::endl;
    if (g_alerting_service) {
        g_alerting_service->stop();
    }
    if (g_server) {
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    using namespace monitoring;

    // 注册信号处理器
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // 获取端口号（默认18888）
    int port = 18888;
    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }

    std::cout << "==================================================" << std::endl;
    std::cout << "  Monitoring Server - Composition Root" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "Initializing components..." << std::endl;

    // ============ 依赖注入 - 创建所有组件 ============

    // 1. 基础设施层：创建仓储实现（Mock版本）
    std::cout << "  [1/4] Creating repositories (Mock)..." << std::endl;
    auto nodeRepository = std::make_unique<infrastructure::MockNodeRepository>();
    auto metricRepository = std::make_unique<infrastructure::MockMetricRepository>();
    auto alertRuleRepository = std::make_unique<infrastructure::MockAlertRuleRepository>();
    auto alertEventRepository = std::make_unique<infrastructure::MockAlertEventRepository>();
    auto notificationService = std::make_unique<infrastructure::MockNotificationService>();

    // 2. 应用层：创建应用服务
    std::cout << "  [2/6] Creating application services..." << std::endl;
    auto ingestionService = std::make_unique<application::IngestionService>(
        nodeRepository.get(),
        metricRepository.get()
    );
    
    auto alertingService = std::make_unique<application::AlertingOrchestratorService>(
        nodeRepository.get(),
        metricRepository.get(),
        alertRuleRepository.get(),
        alertEventRepository.get(),
        notificationService.get(),
        30  // 每30秒检查一次告警
    );

    auto alertRuleManagementService = std::make_unique<application::AlertRuleManagementService>(
        alertRuleRepository.get()
    );

    auto alertEventManagementService = std::make_unique<application::AlertEventManagementService>(
        std::shared_ptr<domain::IAlertEventRepository>(alertEventRepository.get(), [](domain::IAlertEventRepository*){})
    );

    auto queryService = std::make_unique<application::QueryService>(
        nodeRepository.get(),
        metricRepository.get(),
        alertEventRepository.get()
    );

    // 3. 表现层：创建HTTP服务器
    std::cout << "  [3/6] Creating HTTP server..." << std::endl;
    auto httpServer = std::make_unique<presentation::HttpServer>(
        port,
        ingestionService.get(),
        alertRuleManagementService.get(),
        alertEventManagementService.get(),
        queryService.get()
    );

    // 4. 添加示例告警规则
    std::cout << "  [4/6] Adding sample alert rules..." << std::endl;
    
    // 创建CPU告警规则
    domain::AlertExpression cpuExpression;
    cpuExpression.conditions.push_back({
        "cpu.usage_percent", ">", 80.0, "1m"
    });
    cpuExpression.logic = "AND";
    
    domain::AlertRule cpuRule(1, "High CPU Usage", cpuExpression, "WARNING", true, "CPU使用率告警");
    
    // 创建内存告警规则
    domain::AlertExpression memoryExpression;
    memoryExpression.conditions.push_back({
        "memory.usage_percent", ">", 90.0, "2m"
    });
    memoryExpression.logic = "AND";
    
    domain::AlertRule memoryRule(2, "High Memory Usage", memoryExpression, "CRITICAL", true, "内存使用率告警");
    
    alertRuleRepository->save(cpuRule);
    alertRuleRepository->save(memoryRule);
    std::cout << "    ✓ Added CPU usage rule (80% for 1m)" << std::endl;
    std::cout << "    ✓ Added Memory usage rule (90% for 2m)" << std::endl;

    // 5. 启动告警服务
    std::cout << "  [5/6] Starting alerting service..." << std::endl;
    alertingService->start();

    g_server = httpServer.get();
    g_alerting_service = alertingService.get();

    std::cout << "✓ All components initialized successfully!" << std::endl;
    std::cout << "\n==================================================" << std::endl;
    std::cout << "  Component Dependency Graph:" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "HttpServer" << std::endl;
    std::cout << "  └─> IngestionService" << std::endl;
    std::cout << "        ├─> IServerNodeRepository (Mock)" << std::endl;
    std::cout << "        └─> IMetricRepository (Mock)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "AlertingOrchestratorService" << std::endl;
    std::cout << "  ├─> IServerNodeRepository (Mock)" << std::endl;
    std::cout << "  ├─> IMetricRepository (Mock)" << std::endl;
    std::cout << "  ├─> IAlertRuleRepository (Mock)" << std::endl;
    std::cout << "  ├─> IAlertEventRepository (Mock)" << std::endl;
    std::cout << "  └─> INotificationService (Mock)" << std::endl;
    std::cout << "==================================================" << std::endl;

    // ============ 启动服务器 ============
    try {
        httpServer->start();  // 阻塞
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n==================================================" << std::endl;
    std::cout << "  Server stopped gracefully" << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
