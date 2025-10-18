#ifndef DOMAIN_ENTITIES_METRICS_HPP
#define DOMAIN_ENTITIES_METRICS_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace monitoring::domain {

/**
 * CPU资源指标
 */
struct CpuMetrics {
    double usagePercent;      // CPU使用率 (0.0 - 100.0)
    double loadAvg1m;         // 1分钟平均负载
    double loadAvg5m;         // 5分钟平均负载
    double loadAvg15m;        // 15分钟平均负载
    int32_t coreCount;        // CPU核心总数
    int32_t coreAllocated;    // 已分配的核心数
    double temperature;       // 温度 (摄氏度)
    double voltage;           // 电压 (V)
    double current;           // 电流 (A)
    double power;             // 功率 (W)
};

/**
 * 内存资源指标
 */
struct MemoryMetrics {
    uint64_t totalBytes;      // 总内存 (bytes)
    uint64_t usedBytes;       // 已使用内存 (bytes)
    uint64_t freeBytes;       // 空闲内存 (bytes)
    double usagePercent;      // 使用率 (0.0 - 100.0)
};

/**
 * 磁盘分区指标
 */
struct DiskMetrics {
    std::string device;       // 设备名 (e.g., "/dev/sda1")
    std::string mountPoint;   // 挂载点 (e.g., "/")
    uint64_t totalBytes;      // 总容量 (bytes)
    uint64_t usedBytes;       // 已使用 (bytes)
    uint64_t freeBytes;       // 空闲 (bytes)
    double usagePercent;      // 使用率 (0.0 - 100.0)
};

/**
 * 网络接口指标
 */
struct NetworkMetrics {
    std::string interfaceName;  // 网卡名 (e.g., "eth0")
    uint64_t rxBytes;           // 接收字节数
    uint64_t txBytes;           // 发送字节数
    uint64_t rxPackets;         // 接收包数
    uint64_t txPackets;         // 发送包数
    uint64_t rxErrors;          // 接收错误数
    uint64_t txErrors;          // 发送错误数
    uint64_t rxRate;            // 接收速率 (bytes/s)
    uint64_t txRate;            // 发送速率 (bytes/s)
};

/**
 * GPU设备指标
 */
struct GpuMetrics {
    int32_t index;            // GPU索引
    std::string name;         // GPU名称
    double computeUsage;      // 计算使用率 (0.0 - 100.0)
    double memUsage;          // 显存使用率 (0.0 - 100.0)
    uint64_t memUsed;         // 已使用显存 (bytes)
    uint64_t memTotal;        // 总显存 (bytes)
    double temperature;       // 温度 (摄氏度)
    double power;             // 功率 (W)
};

/**
 * 容器组件资源指标
 */
struct ComponentMetrics {
    std::string instanceId;   // 实例ID
    std::string uuid;         // UUID
    int32_t index;            // 索引

    struct Config {
        std::string name;     // 容器名
        std::string id;       // 容器ID
    } config;

    enum class State {
        PENDING,
        RUNNING,
        FAILED,
        STOPPED,
        SLEEPING
    } state;

    struct Resource {
        double cpuLoad;           // CPU负载
        uint64_t memUsed;         // 内存使用 (bytes)
        uint64_t memLimit;        // 内存限制 (bytes)
        uint64_t networkTx;       // 网络发送 (bytes)
        uint64_t networkRx;       // 网络接收 (bytes)
    } resource;
};

/**
 * 节点资源快照 - 核心领域实体
 * 代表某个时间点一台服务器的完整资源快照
 */
class MetricSnapshot {
public:
    // 构造函数
    MetricSnapshot(
        const std::string& nodeId,
        uint64_t timestamp,
        const CpuMetrics& cpu,
        const MemoryMetrics& memory,
        const std::vector<DiskMetrics>& disks,
        const std::vector<NetworkMetrics>& networks,
        const std::vector<GpuMetrics>& gpus,
        int32_t gpuAllocated,
        int32_t gpuNum,
        const std::vector<ComponentMetrics>& components = {}
    );

    // Getters
    const std::string& getNodeId() const { return nodeId_; }
    uint64_t getTimestamp() const { return timestamp_; }
    const CpuMetrics& getCpu() const { return cpu_; }
    const MemoryMetrics& getMemory() const { return memory_; }
    const std::vector<DiskMetrics>& getDisks() const { return disks_; }
    const std::vector<NetworkMetrics>& getNetworks() const { return networks_; }
    const std::vector<GpuMetrics>& getGpus() const { return gpus_; }
    int32_t getGpuAllocated() const { return gpuAllocated_; }
    int32_t getGpuNum() const { return gpuNum_; }
    const std::vector<ComponentMetrics>& getComponents() const { return components_; }

    // 业务方法：根据指标名称提取数值
    double extractMetricValue(const std::string& metricPath) const;

private:
    std::string nodeId_;                        // 关联的服务器节点ID
    uint64_t timestamp_;                        // 数据采集的时间戳 (Unix epoch, UTC)

    CpuMetrics cpu_;                           // CPU指标
    MemoryMetrics memory_;                     // 内存指标
    std::vector<DiskMetrics> disks_;           // 磁盘指标列表
    std::vector<NetworkMetrics> networks_;     // 网络接口指标列表
    std::vector<GpuMetrics> gpus_;             // GPU指标列表
    int32_t gpuAllocated_;                     // 已分配GPU数
    int32_t gpuNum_;                           // GPU总数
    std::vector<ComponentMetrics> components_; // 容器组件指标列表
};

} // namespace monitoring::domain

#endif // DOMAIN_ENTITIES_METRICS_HPP
