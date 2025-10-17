#include "http_server.hpp"
#include "simple_json_parser.hpp"
#include <iostream>

// 需要下载 httplib.h: https://github.com/yhirose/cpp-httplib/blob/master/httplib.h
// 放到 server/presentation/controllers/ 目录下
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

namespace monitoring::presentation {

HttpServer::HttpServer(
    int port,
    application::IngestionService* ingestionService,
    application::AlertRuleManagementService* alertRuleManagementService,
    application::AlertEventManagementService* alertEventManagementService,
    application::QueryService* queryService
)
    : port_(port)
    , ingestionService_(ingestionService)
    , alertRuleManagementService_(alertRuleManagementService)
    , alertEventManagementService_(alertEventManagementService)
    , queryService_(queryService)
    , server_(nullptr)
{
    server_ = new httplib::Server();
    setupRoutes();
}

HttpServer::~HttpServer() {
    if (server_) {
        delete static_cast<httplib::Server*>(server_);
    }
}

void HttpServer::setupRoutes() {
    httplib::Server* svr = static_cast<httplib::Server*>(server_);

    // POST /heartbeat
    svr->Post("/heartbeat", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[Heartbeat] Received request from " << req.remote_addr << std::endl;

        std::string responseBody;
        int statusCode = 200;

        handleHeartbeat(req.body, responseBody, statusCode);

        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // POST /resource
    svr->Post("/resource", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[Resource] Received request from " << req.remote_addr << std::endl;

        std::string responseBody;
        int statusCode = 200;

        handleResource(req.body, responseBody, statusCode);

        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET / - 健康检查
    svr->Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"status":"ok","service":"monitoring-server"})", "application/json");
    });

    // ============ 告警规则管理API ============
    
    // POST /api/alert-rules - 创建告警规则
    svr->Post("/api/alert-rules", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertRule] Create request from " << req.remote_addr << std::endl;
        
        std::string responseBody;
        int statusCode = 200;
        
        handleCreateAlertRule(req.body, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // PUT /api/alert-rules/{id} - 更新告警规则
    svr->Put("/api/alert-rules/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertRule] Update request from " << req.remote_addr << std::endl;
        
        int32_t ruleId = std::stoi(req.matches[1]);
        std::string responseBody;
        int statusCode = 200;
        
        handleUpdateAlertRule(ruleId, req.body, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // DELETE /api/alert-rules/{id} - 删除告警规则
    svr->Delete("/api/alert-rules/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertRule] Delete request from " << req.remote_addr << std::endl;
        
        int32_t ruleId = std::stoi(req.matches[1]);
        std::string responseBody;
        int statusCode = 200;
        
        handleDeleteAlertRule(ruleId, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET /api/alert-rules - 获取告警规则列表
    svr->Get("/api/alert-rules", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[AlertRule] List request" << std::endl;
        
        std::string responseBody;
        int statusCode = 200;
        
        handleGetAlertRules(responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET /api/alert-rules/{id} - 获取告警规则详情
    svr->Get("/api/alert-rules/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertRule] Get request from " << req.remote_addr << std::endl;
        
        int32_t ruleId = std::stoi(req.matches[1]);
        std::string responseBody;
        int statusCode = 200;
        
        handleGetAlertRule(ruleId, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // POST /api/alert-rules/{id}/enable - 启用告警规则
    svr->Post("/api/alert-rules/([0-9]+)/enable", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertRule] Enable request from " << req.remote_addr << std::endl;
        std::cout << "[AlertRule] Request path: " << req.path << std::endl;
        std::cout << "[AlertRule] Matches count: " << req.matches.size() << std::endl;
        
        try {
            int32_t ruleId = std::stoi(req.matches[1]);
            std::cout << "[AlertRule] Enabling rule ID: " << ruleId << std::endl;
            
            std::string responseBody;
            int statusCode = 200;
            
            handleEnableAlertRule(ruleId, responseBody, statusCode);
            
            res.status = statusCode;
            res.set_content(responseBody, "application/json");
            std::cout << "[AlertRule] Enable response: " << responseBody << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[AlertRule] Error in enable handler: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(R"({"code":500,"message":"Internal server error"})", "application/json");
        }
    });

    // 添加一个简单的测试路由
    svr->Post("/test-enable", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[Test] Enable test request from " << req.remote_addr << std::endl;
        res.status = 200;
        res.set_content(R"({"code":0,"message":"Test enable endpoint works"})", "application/json");
    });

    // POST /api/alert-rules/{id}/disable - 禁用告警规则
    svr->Post("/api/alert-rules/([0-9]+)/disable", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertRule] Disable request from " << req.remote_addr << std::endl;
        
        try {
            int32_t ruleId = std::stoi(req.matches[1]);
            std::cout << "[AlertRule] Disabling rule ID: " << ruleId << std::endl;
            
            std::string responseBody;
            int statusCode = 200;
            
            handleDisableAlertRule(ruleId, responseBody, statusCode);
            
            res.status = statusCode;
            res.set_content(responseBody, "application/json");
            std::cout << "[AlertRule] Disable response: " << responseBody << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[AlertRule] Error in disable handler: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(R"({"code":500,"message":"Internal server error"})", "application/json");
        }
    });

    // GET /api/alert-rules/active - 获取启用的告警规则列表
    svr->Get("/api/alert-rules/active", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[AlertRule] Active list request" << std::endl;
        
        std::string responseBody;
        int statusCode = 200;
        
        handleGetActiveAlertRules(responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // ============ 告警事件管理API ============
    
    // GET /api/alert-events - 获取所有告警事件
    svr->Get("/api/alert-events", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[AlertEvent] List request" << std::endl;
        
        std::string responseBody;
        int statusCode = 200;
        
        handleGetAllAlertEvents(responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET /api/alert-events/{id} - 获取告警事件详情
    svr->Get("/api/alert-events/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertEvent] Get request from " << req.remote_addr << std::endl;
        
        int64_t eventId = std::stoll(req.matches[1]);
        std::string responseBody;
        int statusCode = 200;
        
        handleGetAlertEventById(eventId, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET /api/alert-events/node/{nodeId} - 根据节点获取告警事件
    svr->Get("/api/alert-events/node/([^/]+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertEvent] Get by node request from " << req.remote_addr << std::endl;
        
        std::string nodeId = req.matches[1];
        std::string responseBody;
        int statusCode = 200;
        
        handleGetAlertEventsByNode(nodeId, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET /api/alert-events/rule/{ruleId} - 根据规则获取告警事件
    svr->Get("/api/alert-events/rule/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertEvent] Get by rule request from " << req.remote_addr << std::endl;
        
        int32_t ruleId = std::stoi(req.matches[1]);
        std::string responseBody;
        int statusCode = 200;
        
        handleGetAlertEventsByRule(ruleId, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET /api/alert-events/active - 获取活跃的告警事件
    svr->Get("/api/alert-events/active", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[AlertEvent] Active list request" << std::endl;
        
        std::string responseBody;
        int statusCode = 200;
        
        handleGetActiveAlertEvents(responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // GET /api/alert-events/status/{status} - 根据状态获取告警事件
    svr->Get("/api/alert-events/status/([^/]+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertEvent] Get by status request from " << req.remote_addr << std::endl;
        
        std::string status = req.matches[1];
        std::string responseBody;
        int statusCode = 200;
        
        handleGetAlertEventsByStatus(status, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });

    // POST /api/alert-events/{id}/acknowledge - 认知告警事件
    svr->Post("/api/alert-events/(\\d+)/acknowledge", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertEvent] Acknowledge request from " << req.remote_addr << std::endl;
        
        try {
            int64_t eventId = std::stoll(req.matches[1]);
            
            // 从请求体中解析操作员ID
            std::string operatorId = "system"; // 默认值
            if (!req.body.empty()) {
                // 简单的JSON解析，实际项目中应该使用更robust的解析器
                size_t pos = req.body.find("\"operatorId\"");
                if (pos != std::string::npos) {
                    size_t start = req.body.find("\"", pos + 12) + 1;
                    size_t end = req.body.find("\"", start);
                    if (start != std::string::npos && end != std::string::npos) {
                        operatorId = req.body.substr(start, end - start);
                    }
                }
            }
            
            std::string responseBody;
            int statusCode = 200;
            
            handleAcknowledgeAlertEvent(eventId, operatorId, responseBody, statusCode);
            
            res.status = statusCode;
            res.set_content(responseBody, "application/json");
        } catch (const std::exception& e) {
            std::cerr << "[AlertEvent] Error in acknowledge handler: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(R"({"code":500,"message":"Internal server error"})", "application/json");
        }
    });

    // POST /api/alert-events/{id}/resolve - 解决告警事件
    svr->Post("/api/alert-events/(\\d+)/resolve", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[AlertEvent] Resolve request from " << req.remote_addr << std::endl;
        
        try {
            int64_t eventId = std::stoll(req.matches[1]);
            
            std::string responseBody;
            int statusCode = 200;
            
            handleResolveAlertEvent(eventId, responseBody, statusCode);
            
            res.status = statusCode;
            res.set_content(responseBody, "application/json");
        } catch (const std::exception& e) {
            std::cerr << "[AlertEvent] Error in resolve handler: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(R"({"code":500,"message":"Internal server error"})", "application/json");
        }
    });

    std::cout << "Routes registered: POST /heartbeat, POST /resource, GET /" << std::endl;
    std::cout << "Alert Rule API: POST /api/alert-rules, PUT /api/alert-rules/{id}, DELETE /api/alert-rules/{id}" << std::endl;
    std::cout << "Alert Rule API: GET /api/alert-rules, GET /api/alert-rules/{id}, POST /api/alert-rules/{id}/enable, POST /api/alert-rules/{id}/disable" << std::endl;
    std::cout << "Alert Event API: GET /api/alert-events, GET /api/alert-events/{id}, GET /api/alert-events/node/{nodeId}, GET /api/alert-events/rule/{ruleId}" << std::endl;
    std::cout << "Alert Event API: GET /api/alert-events/active, GET /api/alert-events/status/{status}, POST /api/alert-events/{id}/acknowledge, POST /api/alert-events/{id}/resolve" << std::endl;
    
    // ============ 节点和指标查询API ============
    
    // GET /api/nodes - 获取所有节点
    svr->Get("/api/nodes", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[Node] List request" << std::endl;
        
        std::string responseBody;
        int statusCode = 200;
        
        handleGetNodes(responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });
    
    // GET /api/nodes/{nodeId} - 获取节点详情
    svr->Get("/api/nodes/([^/]+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[Node] Get request from " << req.remote_addr << std::endl;
        
        std::string nodeId = req.matches[1];
        std::string responseBody;
        int statusCode = 200;
        
        handleGetNodeById(nodeId, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });
    
    // GET /api/nodes/status/{status} - 根据状态获取节点
    svr->Get("/api/nodes/status/([^/]+)", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[Node] Get by status request from " << req.remote_addr << std::endl;
        
        std::string status = req.matches[1];
        std::string responseBody;
        int statusCode = 200;
        
        handleGetNodesByStatus(status, responseBody, statusCode);
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });
    
    // GET /api/nodes/{nodeId}/metrics - 获取节点指标数据
    svr->Get("/api/nodes/([^/]+)/metrics", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[Metrics] Get request from " << req.remote_addr << std::endl;
        
        std::string nodeId = req.matches[1];
        
        // 解析查询参数
        uint64_t startTime = 0, endTime = 0;
        int32_t recentSeconds = 300; // 默认5分钟
        
        if (req.has_param("startTime")) {
            startTime = std::stoull(req.get_param_value("startTime"));
        }
        if (req.has_param("endTime")) {
            endTime = std::stoull(req.get_param_value("endTime"));
        }
        if (req.has_param("recentSeconds")) {
            recentSeconds = std::stoi(req.get_param_value("recentSeconds"));
        }
        
        std::string responseBody;
        int statusCode = 200;
        
        if (startTime > 0 && endTime > 0) {
            handleGetMetrics(nodeId, startTime, endTime, responseBody, statusCode);
        } else {
            handleGetRecentMetrics(nodeId, recentSeconds, responseBody, statusCode);
        }
        
        res.status = statusCode;
        res.set_content(responseBody, "application/json");
    });
    
    std::cout << "Node API: GET /api/nodes, GET /api/nodes/{nodeId}, GET /api/nodes/status/{status}" << std::endl;
    std::cout << "Metrics API: GET /api/nodes/{nodeId}/metrics" << std::endl;
}

void HttpServer::handleHeartbeat(const std::string& body, std::string& response, int& statusCode) {
    try {
        // 1. 解析JSON -> DTO
        auto dto = SimpleJsonParser::parseHeartbeatRequest(body);

        // 2. 调用应用层服务
        auto responseDto = ingestionService_->processHeartbeat(dto);

        // 3. 序列化响应
        response = SimpleJsonParser::serializeHeartbeatResponse(responseDto);

        statusCode = (responseDto.code == 0) ? 200 : 500;

        std::cout << "  ✓ Heartbeat processed: node_id=" << dto.data.host_ip
                  << ", hostname=" << dto.data.hostname << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error processing heartbeat: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleResource(const std::string& body, std::string& response, int& statusCode) {
    try {
        // 1. 解析JSON -> DTO
        auto dto = SimpleJsonParser::parseResourceRequest(body);

        // 2. 调用应用层服务
        auto responseDto = ingestionService_->processResourceReport(dto);

        // 3. 序列化响应
        response = SimpleJsonParser::serializeResourceResponse(responseDto);

        statusCode = (responseDto.code == 0) ? 200 : (responseDto.code == 404 ? 404 : 500);

        std::cout << "  ✓ Resource processed: host_ip=" << dto.data.host_ip
                  << ", cpu=" << dto.data.resource.cpu.usage_percent << "%" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error processing resource: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::start() {
    httplib::Server* svr = static_cast<httplib::Server*>(server_);

    std::cout << "\n========================================" << std::endl;
    std::cout << "  Monitoring Server Starting..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Listening on http://0.0.0.0:" << port_ << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 阻塞监听
    if (!svr->listen("0.0.0.0", port_)) {
        std::cerr << "Failed to start server on port " << port_ << std::endl;
    }
}

void HttpServer::stop() {
    httplib::Server* svr = static_cast<httplib::Server*>(server_);
    svr->stop();
    std::cout << "\nServer stopped" << std::endl;
}

// ============ 告警规则管理API处理函数实现 ============

void HttpServer::handleCreateAlertRule(const std::string& body, std::string& response, int& statusCode) {
    try {
        // 1. 解析JSON -> DTO
        auto request = SimpleJsonParser::parseCreateAlertRuleRequest(body);

        // 2. 调用应用层服务
        auto responseDto = alertRuleManagementService_->createAlertRule(request);

        // 3. 序列化响应
        response = SimpleJsonParser::serializeAlertRuleDetailResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 201 : (responseDto.code == 400 ? 400 : 500);

        std::cout << "  ✓ Alert rule created: " << request.ruleName << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error creating alert rule: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleUpdateAlertRule(int32_t ruleId, const std::string& body, std::string& response, int& statusCode) {
    try {
        // 1. 解析JSON -> DTO
        auto request = SimpleJsonParser::parseUpdateAlertRuleRequest(body);

        // 2. 调用应用层服务
        auto responseDto = alertRuleManagementService_->updateAlertRule(ruleId, request);

        // 3. 序列化响应
        response = SimpleJsonParser::serializeAlertRuleDetailResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : (responseDto.code == 404 ? 404 : 500);

        std::cout << "  ✓ Alert rule updated: " << ruleId << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error updating alert rule: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleDeleteAlertRule(int32_t ruleId, std::string& response, int& statusCode) {
    try {
        // 1. 调用应用层服务
        auto responseDto = alertRuleManagementService_->deleteAlertRule(ruleId);

        // 2. 序列化响应
        response = SimpleJsonParser::serializeCommonResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : (responseDto.code == 404 ? 404 : 500);

        std::cout << "  ✓ Alert rule deleted: " << ruleId << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error deleting alert rule: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetAlertRules(std::string& response, int& statusCode) {
    try {
        // 1. 调用应用层服务
        auto responseDto = alertRuleManagementService_->getAlertRules();

        // 2. 序列化响应
        response = SimpleJsonParser::serializeAlertRuleListResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 500;

        std::cout << "  ✓ Alert rules retrieved: " << responseDto.total << " rules" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving alert rules: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetAlertRule(int32_t ruleId, std::string& response, int& statusCode) {
    try {
        // 1. 调用应用层服务
        auto responseDto = alertRuleManagementService_->getAlertRule(ruleId);

        // 2. 序列化响应
        response = SimpleJsonParser::serializeAlertRuleDetailResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : (responseDto.code == 404 ? 404 : 500);

        std::cout << "  ✓ Alert rule retrieved: " << ruleId << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving alert rule: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleEnableAlertRule(int32_t ruleId, std::string& response, int& statusCode) {
    try {
        // 1. 调用应用层服务
        auto responseDto = alertRuleManagementService_->enableAlertRule(ruleId);

        // 2. 序列化响应
        response = SimpleJsonParser::serializeCommonResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : (responseDto.code == 404 ? 404 : 500);

        std::cout << "  ✓ Alert rule enabled: " << ruleId << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error enabling alert rule: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleDisableAlertRule(int32_t ruleId, std::string& response, int& statusCode) {
    try {
        // 1. 调用应用层服务
        auto responseDto = alertRuleManagementService_->disableAlertRule(ruleId);

        // 2. 序列化响应
        response = SimpleJsonParser::serializeCommonResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : (responseDto.code == 404 ? 404 : 500);

        std::cout << "  ✓ Alert rule disabled: " << ruleId << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error disabling alert rule: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetActiveAlertRules(std::string& response, int& statusCode) {
    try {
        // 1. 调用应用层服务
        auto responseDto = alertRuleManagementService_->getActiveAlertRules();

        // 2. 序列化响应
        response = SimpleJsonParser::serializeAlertRuleListResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 500;

        std::cout << "  ✓ Active alert rules retrieved: " << responseDto.total << " rules" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving active alert rules: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

// ============ 告警事件管理API处理函数实现 ============

void HttpServer::handleGetAllAlertEvents(std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->getAllAlertEvents();
        
        // 序列化响应
        response = SimpleJsonParser::serializeAlertEventListResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 500;
        
        std::cout << "  ✓ Alert events retrieved: " << responseDto.total << " events" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving alert events: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetAlertEventById(int64_t eventId, std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->getAlertEventById(eventId);
        
        // 序列化响应
        response = SimpleJsonParser::serializeAlertEventDetailResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 404;
        
        std::cout << "  ✓ Alert event retrieved: " << eventId << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving alert event " << eventId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetAlertEventsByNode(const std::string& nodeId, std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->getAlertEventsByNode(nodeId);
        
        // 序列化响应
        response = SimpleJsonParser::serializeAlertEventListResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 500;
        
        std::cout << "  ✓ Alert events for node retrieved: " << nodeId << " (" << responseDto.total << " events)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving alert events for node " << nodeId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetAlertEventsByRule(int32_t ruleId, std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->getAlertEventsByRule(ruleId);
        
        // 序列化响应
        response = SimpleJsonParser::serializeAlertEventListResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 500;
        
        std::cout << "  ✓ Alert events for rule retrieved: " << ruleId << " (" << responseDto.total << " events)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving alert events for rule " << ruleId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetActiveAlertEvents(std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->getActiveAlertEvents();
        
        // 序列化响应
        response = SimpleJsonParser::serializeAlertEventListResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 500;
        
        std::cout << "  ✓ Active alert events retrieved: " << responseDto.total << " events" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving active alert events: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetAlertEventsByStatus(const std::string& status, std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->getAlertEventsByStatus(status);
        
        // 序列化响应
        response = SimpleJsonParser::serializeAlertEventListResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 400;
        
        std::cout << "  ✓ Alert events by status retrieved: " << status << " (" << responseDto.total << " events)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving alert events by status " << status << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleAcknowledgeAlertEvent(int64_t eventId, const std::string& operatorId, std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->acknowledgeAlertEvent(eventId, operatorId);
        
        // 序列化响应
        response = SimpleJsonParser::serializeCommonResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 404;
        
        std::cout << "  ✓ Alert event acknowledged: " << eventId << " by " << operatorId << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error acknowledging alert event " << eventId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleResolveAlertEvent(int64_t eventId, std::string& response, int& statusCode) {
    try {
        // 调用应用层服务
        auto responseDto = alertEventManagementService_->resolveAlertEvent(eventId);
        
        // 序列化响应
        response = SimpleJsonParser::serializeCommonResponse(responseDto);
        statusCode = (responseDto.code == 0) ? 200 : 404;
        
        std::cout << "  ✓ Alert event resolved: " << eventId << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error resolving alert event " << eventId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

// ============ 节点和指标查询API处理函数实现 ============

void HttpServer::handleGetNodes(std::string& response, int& statusCode) {
    try {
        auto nodes = queryService_->getNodes();
        
        // 构建JSON响应
        std::ostringstream oss;
        oss << "{";
        oss << "\"code\":0,";
        oss << "\"message\":\"Success\",";
        oss << "\"total\":" << nodes.size() << ",";
        oss << "\"nodes\":[";
        
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& node = nodes[i];
            oss << "{";
            oss << "\"nodeId\":\"" << node.nodeId << "\",";
            oss << "\"hostname\":\"" << node.hostname << "\",";
            oss << "\"ipAddress\":\"" << node.ipAddress << "\",";
            oss << "\"status\":\"" << node.status << "\",";
            oss << "\"lastSeenAt\":" << node.lastSeenAt << ",";
            oss << "\"isOnline\":" << (node.isOnline ? "true" : "false");
            oss << "}";
        }
        
        oss << "]";
        oss << "}";
        
        response = oss.str();
        statusCode = 200;
        
        std::cout << "  ✓ Nodes retrieved: " << nodes.size() << " nodes" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving nodes: " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetNodeById(const std::string& nodeId, std::string& response, int& statusCode) {
    try {
        auto node = queryService_->getNodeById(nodeId);
        
        if (!node.has_value()) {
            response = R"({"code":404,"message":"Node not found"})";
            statusCode = 404;
            return;
        }
        
        // 构建JSON响应
        std::ostringstream oss;
        oss << "{";
        oss << "\"code\":0,";
        oss << "\"message\":\"Success\",";
        oss << "\"node\":{";
        oss << "\"nodeId\":\"" << node->nodeId << "\",";
        oss << "\"hostname\":\"" << node->hostname << "\",";
        oss << "\"ipAddress\":\"" << node->ipAddress << "\",";
        oss << "\"status\":\"" << node->status << "\",";
        oss << "\"lastSeenAt\":" << node->lastSeenAt << ",";
        oss << "\"isOnline\":" << (node->isOnline ? "true" : "false");
        oss << "}";
        oss << "}";
        
        response = oss.str();
        statusCode = 200;
        
        std::cout << "  ✓ Node retrieved: " << nodeId << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving node " << nodeId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetNodesByStatus(const std::string& status, std::string& response, int& statusCode) {
    try {
        auto nodes = queryService_->getNodesByStatus(status);
        
        // 构建JSON响应
        std::ostringstream oss;
        oss << "{";
        oss << "\"code\":0,";
        oss << "\"message\":\"Success\",";
        oss << "\"total\":" << nodes.size() << ",";
        oss << "\"nodes\":[";
        
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& node = nodes[i];
            oss << "{";
            oss << "\"nodeId\":\"" << node.nodeId << "\",";
            oss << "\"hostname\":\"" << node.hostname << "\",";
            oss << "\"ipAddress\":\"" << node.ipAddress << "\",";
            oss << "\"status\":\"" << node.status << "\",";
            oss << "\"lastSeenAt\":" << node.lastSeenAt << ",";
            oss << "\"isOnline\":" << (node.isOnline ? "true" : "false");
            oss << "}";
        }
        
        oss << "]";
        oss << "}";
        
        response = oss.str();
        statusCode = 200;
        
        std::cout << "  ✓ Nodes by status retrieved: " << status << " (" << nodes.size() << " nodes)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving nodes by status " << status << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetMetrics(const std::string& nodeId, uint64_t startTime, uint64_t endTime, std::string& response, int& statusCode) {
    try {
        auto metrics = queryService_->getMetrics(nodeId, startTime, endTime);
        
        // 构建JSON响应
        std::ostringstream oss;
        oss << "{";
        oss << "\"code\":0,";
        oss << "\"message\":\"Success\",";
        oss << "\"total\":" << metrics.size() << ",";
        oss << "\"metrics\":[";
        
        for (size_t i = 0; i < metrics.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& metric = metrics[i];
            oss << "{";
            oss << "\"timestamp\":" << metric.timestamp << ",";
            oss << "\"cpuUsage\":" << metric.cpuUsage << ",";
            oss << "\"memoryUsage\":" << metric.memoryUsage << ",";
            oss << "\"diskUsage\":" << metric.diskUsage << ",";
            oss << "\"networkRxRate\":" << metric.networkRxRate << ",";
            oss << "\"networkTxRate\":" << metric.networkTxRate;
            oss << "}";
        }
        
        oss << "]";
        oss << "}";
        
        response = oss.str();
        statusCode = 200;
        
        std::cout << "  ✓ Metrics retrieved: " << nodeId << " (" << metrics.size() << " data points)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving metrics for " << nodeId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

void HttpServer::handleGetRecentMetrics(const std::string& nodeId, int32_t recentSeconds, std::string& response, int& statusCode) {
    try {
        auto metrics = queryService_->getRecentMetrics(nodeId, recentSeconds);
        
        // 构建JSON响应
        std::ostringstream oss;
        oss << "{";
        oss << "\"code\":0,";
        oss << "\"message\":\"Success\",";
        oss << "\"total\":" << metrics.size() << ",";
        oss << "\"metrics\":[";
        
        for (size_t i = 0; i < metrics.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& metric = metrics[i];
            oss << "{";
            oss << "\"timestamp\":" << metric.timestamp << ",";
            oss << "\"cpuUsage\":" << metric.cpuUsage << ",";
            oss << "\"memoryUsage\":" << metric.memoryUsage << ",";
            oss << "\"diskUsage\":" << metric.diskUsage << ",";
            oss << "\"networkRxRate\":" << metric.networkRxRate << ",";
            oss << "\"networkTxRate\":" << metric.networkTxRate;
            oss << "}";
        }
        
        oss << "]";
        oss << "}";
        
        response = oss.str();
        statusCode = 200;
        
        std::cout << "  ✓ Recent metrics retrieved: " << nodeId << " (" << metrics.size() << " data points)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error retrieving recent metrics for " << nodeId << ": " << e.what() << std::endl;
        response = R"({"code":500,"message":"Internal server error"})";
        statusCode = 500;
    }
}

} // namespace monitoring::presentation
