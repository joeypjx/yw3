#ifndef DOMAIN_ENTITIES_SERVER_NODE_HPP
#define DOMAIN_ENTITIES_SERVER_NODE_HPP

#include <string>
#include <cstdint>
#include <chrono>

namespace monitoring::domain {

/**
 * ServerNode - 服务器节点实体
 * 代表一台被监控的服务器，包含其元数据和健康状态
 */
class ServerNode {
public:
    /**
     * 节点健康状态枚举
     */
    enum class Status {
        UNKNOWN,    // 未知状态（刚注册或长时间未上报）
        HEALTHY,    // 健康
        WARNING,    // 警告（轻微问题）
        CRITICAL    // 严重（需要立即关注）
    };

    /**
     * 节点硬件信息
     */
    struct HardwareInfo {
        int32_t boxId;              // 机箱号
        int32_t slotId;             // 槽位号
        int32_t cpuId;              // CPU号
        int32_t srioId;             // SRIO号
        uint16_t servicePort;       // 命令响应服务端口
        std::string boxType;        // 机箱类型
        std::string boardType;      // 板卡类型
        std::string cpuType;        // CPU类型
        std::string osType;         // 操作系统类型
        std::string resourceType;   // 资源类型
        std::string cpuArch;        // CPU架构

        // GPU信息列表
        struct GpuDevice {
            int32_t index;
            std::string name;
        };
        std::vector<GpuDevice> gpus;
    };

    // 构造函数
    ServerNode(
        const std::string& nodeId,
        const std::string& hostname,
        const std::string& ipAddress,
        const HardwareInfo& hwInfo
    );

    // Getters
    const std::string& getNodeId() const { return nodeId_; }
    const std::string& getHostname() const { return hostname_; }
    const std::string& getIpAddress() const { return ipAddress_; }
    const HardwareInfo& getHardwareInfo() const { return hardwareInfo_; }
    Status getStatus() const { return status_; }
    uint64_t getCreatedAt() const { return createdAt_; }
    uint64_t getLastSeenAt() const { return lastSeenAt_; }

    // 业务方法

    /**
     * 更新最后心跳时间
     */
    void updateLastSeen();

    /**
     * 更新节点状态
     */
    void updateStatus(Status newStatus);

    /**
     * 判断节点是否在线
     * @param timeoutSeconds 超时时间（秒）
     * @return 如果在超时时间内有心跳则返回true
     */
    bool isOnline(uint64_t timeoutSeconds = 60) const;

    /**
     * 获取节点唯一标识
     * 根据 IP + 硬件信息生成
     */
    static std::string generateNodeId(const std::string& ipAddress, const HardwareInfo& hwInfo);

private:
    std::string nodeId_;            // 节点唯一标识
    std::string hostname_;          // 主机名
    std::string ipAddress_;         // IP地址
    HardwareInfo hardwareInfo_;     // 硬件信息
    Status status_;                 // 健康状态
    uint64_t createdAt_;            // 创建时间戳
    uint64_t lastSeenAt_;           // 最后心跳时间戳

    // 获取当前Unix时间戳（秒）
    static uint64_t getCurrentTimestamp();
};

// 状态转字符串
std::string statusToString(ServerNode::Status status);
ServerNode::Status stringToStatus(const std::string& str);

} // namespace monitoring::domain

#endif // DOMAIN_ENTITIES_SERVER_NODE_HPP
