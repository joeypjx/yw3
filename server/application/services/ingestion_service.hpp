#ifndef APPLICATION_SERVICES_INGESTION_SERVICE_HPP
#define APPLICATION_SERVICES_INGESTION_SERVICE_HPP

#include "../dto/heartbeat_dto.hpp"
#include "../dto/resource_dto.hpp"
#include "../../domain/repositories/i_server_node_repository.hpp"
#include "../../domain/repositories/i_metric_repository.hpp"
#include <memory>

namespace monitoring::application {

/**
 * IngestionService - 数据接收服务
 *
 * 职责：
 * 1. 处理Agent上报的心跳数据
 * 2. 处理Agent上报的资源指标数据
 * 3. 将DTO转换为领域对象
 * 4. 调用仓储进行持久化
 *
 * 这是一个应用服务，负责用例编排，不包含业务逻辑
 */
class IngestionService {
public:
    /**
     * 构造函数 - 依赖注入
     */
    IngestionService(
        domain::IServerNodeRepository* nodeRepository,
        domain::IMetricRepository* metricRepository
    );

    /**
     * 处理心跳上报
     *
     * @param dto 心跳请求DTO
     * @return 心跳响应DTO
     */
    HeartbeatResponseDTO processHeartbeat(const HeartbeatRequestDTO& dto);

    /**
     * 处理资源上报
     *
     * @param dto 资源上报请求DTO
     * @return 资源上报响应DTO
     */
    ResourceReportResponseDTO processResourceReport(const ResourceReportRequestDTO& dto);

private:
    domain::IServerNodeRepository* nodeRepository_;
    domain::IMetricRepository* metricRepository_;
};

} // namespace monitoring::application

#endif // APPLICATION_SERVICES_INGESTION_SERVICE_HPP
