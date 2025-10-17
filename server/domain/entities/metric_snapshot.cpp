#include "metrics.hpp"
#include <stdexcept>

namespace monitoring::domain {

MetricSnapshot::MetricSnapshot(
    const std::string& nodeId,
    uint64_t timestamp,
    const CpuMetrics& cpu,
    const MemoryMetrics& memory,
    const std::vector<DiskMetrics>& disks,
    const std::vector<NetworkMetrics>& networks,
    const std::vector<GpuMetrics>& gpus,
    int32_t gpuAllocated,
    int32_t gpuNum,
    const std::vector<ComponentMetrics>& components
)
    : nodeId_(nodeId)
    , timestamp_(timestamp)
    , cpu_(cpu)
    , memory_(memory)
    , disks_(disks)
    , networks_(networks)
    , gpus_(gpus)
    , gpuAllocated_(gpuAllocated)
    , gpuNum_(gpuNum)
    , components_(components)
{
}

double MetricSnapshot::extractMetricValue(const std::string& metricPath) const {
    // 支持形如 "cpu.usage_percent", "memory.usage_percent" 的路径
    if (metricPath == "cpu.usage_percent") {
        return cpu_.usagePercent;
    } else if (metricPath == "cpu.load_avg_1m") {
        return cpu_.loadAvg1m;
    } else if (metricPath == "cpu.temperature") {
        return cpu_.temperature;
    } else if (metricPath == "memory.usage_percent") {
        return memory_.usagePercent;
    } else if (metricPath == "memory.used") {
        return static_cast<double>(memory_.usedBytes);
    } else {
        throw std::invalid_argument("Unknown metric path: " + metricPath);
    }
}

} // namespace monitoring::domain
