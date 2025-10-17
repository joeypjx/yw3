#ifndef APPLICATION_DTO_RESOURCE_DTO_HPP
#define APPLICATION_DTO_RESOURCE_DTO_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace monitoring::application {

/**
 * ResourceDTO - 资源上报的数据传输对象
 * 对应 resource.yaml 的请求结构
 */

/**
 * CPU资源DTO
 */
struct CpuResourceDTO {
    double usage_percent;
    double load_avg_1m;
    double load_avg_5m;
    double load_avg_15m;
    int32_t core_count;
    int32_t core_allocated;
    double temperature;
    double voltage;
    double current;
    double power;
};

/**
 * 内存资源DTO
 */
struct MemoryResourceDTO {
    uint64_t total;
    uint64_t used;
    uint64_t free;
    double usage_percent;
};

/**
 * 网络接口DTO
 */
struct NetworkInterfaceDTO {
    std::string interface;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_errors;
    uint64_t tx_errors;
    uint64_t rx_rate;
    uint64_t tx_rate;
};

/**
 * 磁盘分区DTO
 */
struct DiskPartitionDTO {
    std::string device;
    std::string mount_point;
    uint64_t total;
    uint64_t used;
    uint64_t free;
    double usage_percent;
};

/**
 * GPU资源DTO
 */
struct GpuResourceDTO {
    int32_t index;
    std::string name;
    double compute_usage;
    double mem_usage;
    uint64_t mem_used;
    uint64_t mem_total;
    double temperature;
    double power;
};

/**
 * 组件资源DTO
 */
struct ComponentResourceDTO {
    std::string instance_id;
    std::string uuid;
    int32_t index;

    struct ConfigDTO {
        std::string name;
        std::string id;
    } config;

    std::string state;  // "PENDING", "RUNNING", "FAILED", "STOPPED", "SLEEPING"

    struct ResourceDTO {
        double cpu_load;
        uint64_t mem_used;
        uint64_t mem_limit;
        uint64_t network_tx;
        uint64_t network_rx;
    } resource;
};

/**
 * 节点资源DTO
 */
struct NodeResourceDTO {
    CpuResourceDTO cpu;
    MemoryResourceDTO memory;
    std::vector<NetworkInterfaceDTO> network;
    std::vector<DiskPartitionDTO> disk;
    std::vector<GpuResourceDTO> gpu;
    int32_t gpu_allocated;
    int32_t gpu_num;
};

/**
 * 资源上报负载DTO
 */
struct ResourcePayloadDTO {
    std::string host_ip;
    NodeResourceDTO resource;
    std::vector<ComponentResourceDTO> component;  // 可选
};

/**
 * 资源上报请求DTO
 */
struct ResourceReportRequestDTO {
    int32_t api_version;
    ResourcePayloadDTO data;
};

/**
 * 资源上报响应DTO
 */
struct ResourceReportResponseDTO {
    int32_t code;           // 0表示成功
    std::string message;
};

} // namespace monitoring::application

#endif // APPLICATION_DTO_RESOURCE_DTO_HPP
