#ifndef APPLICATION_DTO_HEARTBEAT_DTO_HPP
#define APPLICATION_DTO_HEARTBEAT_DTO_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace monitoring::application {

/**
 * HeartbeatDTO - 心跳上报的数据传输对象
 * 对应 node.yaml 的请求结构
 * 这是一个纯数据结构，用于HTTP层和应用层之间传递数据
 */

/**
 * GPU设备信息
 */
struct GpuDeviceDTO {
    int32_t index;
    std::string name;
};

/**
 * 系统硬件信息
 */
struct SystemInfoDTO {
    int32_t box_id;
    int32_t slot_id;
    int32_t cpu_id;
    int32_t srio_id;
    std::string host_ip;
    std::string hostname;
    uint16_t service_port;
    std::string box_type;
    std::string board_type;
    std::string cpu_type;
    std::string os_type;
    std::string resource_type;
    std::string cpu_arch;
    std::vector<GpuDeviceDTO> gpu;
};

/**
 * 心跳请求DTO
 */
struct HeartbeatRequestDTO {
    int32_t api_version;
    SystemInfoDTO data;
};

/**
 * 心跳响应DTO
 */
struct HeartbeatResponseDTO {
    int32_t code;           // 0表示成功
    std::string message;
    // 可选的data字段可以用 std::optional 或省略
};

} // namespace monitoring::application

#endif // APPLICATION_DTO_HEARTBEAT_DTO_HPP
