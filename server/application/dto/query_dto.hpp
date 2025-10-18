#ifndef APPLICATION_DTO_QUERY_DTO_HPP
#define APPLICATION_DTO_QUERY_DTO_HPP

#include <string>
#include <cstdint>

namespace monitoring::application {

/**
 * 节点健康状态DTO
 */
struct NodeHealthDTO {
    std::string nodeId;
    std::string hostname;
    std::string ipAddress;
    std::string status;  // "HEALTHY", "WARNING", "CRITICAL", "UNKNOWN"
    uint64_t lastSeenAt;
    bool isOnline;
};

/**
 * 指标数据DTO
 */
struct MetricDataDTO {
    uint64_t timestamp;
    double cpuUsage;
    double memoryUsage;
    double diskUsage;
    double networkRxRate;
    double networkTxRate;
};

} // namespace monitoring::application

#endif // APPLICATION_DTO_QUERY_DTO_HPP
