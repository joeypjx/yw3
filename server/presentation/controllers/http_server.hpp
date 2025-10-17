#ifndef PRESENTATION_CONTROLLERS_HTTP_SERVER_HPP
#define PRESENTATION_CONTROLLERS_HTTP_SERVER_HPP

#include <memory>
#include <string>
#include "../../application/services/ingestion_service.hpp"
#include "../../application/services/alert_rule_management_service.hpp"
#include "../../application/services/alert_event_management_service.hpp"
#include "../../application/services/query_service.hpp"

namespace monitoring::presentation {

/**
 * HttpServer - HTTP服务器封装
 *
 * 使用 cpp-httplib 提供HTTP服务
 * 注意：需要下载 httplib.h 单头文件
 * https://github.com/yhirose/cpp-httplib
 */
class HttpServer {
public:
    /**
     * 构造函数
     * @param port 监听端口
     * @param ingestionService 数据接收服务
     * @param alertRuleManagementService 告警规则管理服务
     * @param alertEventManagementService 告警事件管理服务
     * @param queryService 查询服务
     */
    HttpServer(
        int port,
        application::IngestionService* ingestionService,
        application::AlertRuleManagementService* alertRuleManagementService,
        application::AlertEventManagementService* alertEventManagementService,
        application::QueryService* queryService
    );

    ~HttpServer();

    /**
     * 启动服务器（阻塞）
     */
    void start();

    /**
     * 停止服务器
     */
    void stop();

private:
    int port_;
    application::IngestionService* ingestionService_;
    application::AlertRuleManagementService* alertRuleManagementService_;
    application::AlertEventManagementService* alertEventManagementService_;
    application::QueryService* queryService_;
    void* server_;  // httplib::Server* (避免头文件依赖)

    // 注册路由
    void setupRoutes();

    // 路由处理函数
    void handleHeartbeat(const std::string& body, std::string& response, int& statusCode);
    void handleResource(const std::string& body, std::string& response, int& statusCode);
    
    // 告警规则管理API处理函数
    void handleCreateAlertRule(const std::string& body, std::string& response, int& statusCode);
    void handleUpdateAlertRule(int32_t ruleId, const std::string& body, std::string& response, int& statusCode);
    void handleDeleteAlertRule(int32_t ruleId, std::string& response, int& statusCode);
    void handleGetAlertRules(std::string& response, int& statusCode);
    void handleGetAlertRule(int32_t ruleId, std::string& response, int& statusCode);
    void handleEnableAlertRule(int32_t ruleId, std::string& response, int& statusCode);
    void handleDisableAlertRule(int32_t ruleId, std::string& response, int& statusCode);
    void handleGetActiveAlertRules(std::string& response, int& statusCode);
    
    // 告警事件管理API处理函数
    void handleGetAllAlertEvents(std::string& response, int& statusCode);
    void handleGetAlertEventById(int64_t eventId, std::string& response, int& statusCode);
    void handleGetAlertEventsByNode(const std::string& nodeId, std::string& response, int& statusCode);
    void handleGetAlertEventsByRule(int32_t ruleId, std::string& response, int& statusCode);
    void handleGetActiveAlertEvents(std::string& response, int& statusCode);
    void handleGetAlertEventsByStatus(const std::string& status, std::string& response, int& statusCode);
    void handleAcknowledgeAlertEvent(int64_t eventId, const std::string& operatorId, std::string& response, int& statusCode);
    void handleResolveAlertEvent(int64_t eventId, std::string& response, int& statusCode);
    
    // 节点和指标查询API处理函数
    void handleGetNodes(std::string& response, int& statusCode);
    void handleGetNodeById(const std::string& nodeId, std::string& response, int& statusCode);
    void handleGetNodesByStatus(const std::string& status, std::string& response, int& statusCode);
    void handleGetMetrics(const std::string& nodeId, uint64_t startTime, uint64_t endTime, std::string& response, int& statusCode);
    void handleGetRecentMetrics(const std::string& nodeId, int32_t recentSeconds, std::string& response, int& statusCode);
};

} // namespace monitoring::presentation

#endif // PRESENTATION_CONTROLLERS_HTTP_SERVER_HPP
