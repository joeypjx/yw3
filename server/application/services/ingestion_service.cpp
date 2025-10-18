#include "ingestion_service.hpp"
#include "../dto/dto_converter.hpp"
#include "../../domain/entities/server_node.hpp"
#include "../../domain/entities/metric_snapshot.hpp"
#include <iostream>

namespace monitoring::application {

IngestionService::IngestionService(
    domain::IServerNodeRepository* nodeRepository,
    domain::IMetricRepository* metricRepository
)
    : nodeRepository_(nodeRepository)
    , metricRepository_(metricRepository)
{
}

HeartbeatResponseDTO IngestionService::processHeartbeat(const HeartbeatRequestDTO& dto) {
    HeartbeatResponseDTO response;

    try {
        // 1. 将DTO转换为领域对象
        domain::ServerNode node = DTOConverter::toServerNode(dto);
        std::cout << "[IngestionService] 💓 Processing heartbeat for node: " << node.getNodeId() << std::endl;

        // 2. 检查节点是否已存在
        auto existingNode = nodeRepository_->findById(node.getNodeId());

        if (existingNode.has_value()) {
            // 节点已存在，更新最后心跳时间和状态
            std::cout << "[IngestionService] 🔄 Updating existing node: " << node.getNodeId() << std::endl;
            existingNode->updateLastSeen();
            existingNode->updateStatus(domain::ServerNode::Status::HEALTHY);
            nodeRepository_->save(*existingNode);
        } else {
            // 新节点，直接保存
            std::cout << "[IngestionService] ✨ Creating new node: " << node.getNodeId() << std::endl;
            nodeRepository_->save(node);
        }

        response.code = 0;
        response.message = "success";

    } catch (const std::exception& e) {
        std::cerr << "[IngestionService] ❌ Error processing heartbeat: " << e.what() << std::endl;
        response.code = 500;
        response.message = std::string("Internal error: ") + e.what();
    }

    return response;
}

ResourceReportResponseDTO IngestionService::processResourceReport(
    const ResourceReportRequestDTO& dto
) {
    ResourceReportResponseDTO response;

    try {
        // 1. 根据IP地址查找节点
        auto nodes = nodeRepository_->findByIpAddress(dto.data.host_ip);
        std::cout << "[IngestionService] 📊 Processing resource report for IP: " << dto.data.host_ip << std::endl;

        if (nodes.empty()) {
            std::cout << "[IngestionService] ❌ Node not found for IP: " << dto.data.host_ip << std::endl;
            response.code = 404;
            response.message = "Node not found. Please send heartbeat first.";
            return response;
        }

        // 2. 获取节点ID
        std::string nodeId = nodes[0].getNodeId();
        std::cout << "[IngestionService] 🔍 Found node: " << nodeId << " for IP: " << dto.data.host_ip << std::endl;

        // 3. 将DTO转换为MetricSnapshot领域对象
        domain::MetricSnapshot snapshot = DTOConverter::toMetricSnapshot(dto, nodeId);

        // 4. 保存指标数据到时序数据库
        metricRepository_->save(snapshot);

        // 5. 更新节点的最后心跳时间
        nodes[0].updateLastSeen();
        nodeRepository_->save(nodes[0]);

        response.code = 0;
        response.message = "success";

    } catch (const std::exception& e) {
        std::cerr << "[IngestionService] ❌ Error processing resource report: " << e.what() << std::endl;
        response.code = 500;
        response.message = std::string("Internal error: ") + e.what();
    }

    return response;
}

} // namespace monitoring::application
