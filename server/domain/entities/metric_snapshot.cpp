#include "metric_snapshot.hpp"
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
    // ============ CPU指标 ============
    if (metricPath == "cpu.usage_percent") {
        return cpu_.usagePercent;
    } else if (metricPath == "cpu.load_avg_1m") {
        return cpu_.loadAvg1m;
    } else if (metricPath == "cpu.load_avg_5m") {
        return cpu_.loadAvg5m;
    } else if (metricPath == "cpu.load_avg_15m") {
        return cpu_.loadAvg15m;
    } else if (metricPath == "cpu.temperature") {
        return cpu_.temperature;
    } else if (metricPath == "cpu.voltage") {
        return cpu_.voltage;
    } else if (metricPath == "cpu.current") {
        return cpu_.current;
    } else if (metricPath == "cpu.power") {
        return cpu_.power;
    } else if (metricPath == "cpu.core_count") {
        return static_cast<double>(cpu_.coreCount);
    } else if (metricPath == "cpu.core_allocated") {
        return static_cast<double>(cpu_.coreAllocated);
    }
    
    // ============ 内存指标 ============
    else if (metricPath == "memory.usage_percent") {
        return memory_.usagePercent;
    } else if (metricPath == "memory.used") {
        return static_cast<double>(memory_.usedBytes);
    } else if (metricPath == "memory.free") {
        return static_cast<double>(memory_.freeBytes);
    } else if (metricPath == "memory.total") {
        return static_cast<double>(memory_.totalBytes);
    }
    
    // ============ 磁盘指标 ============
    // 磁盘聚合指标
    else if (metricPath == "disk.usage_percent.avg") {
        if (disks_.empty()) return 0.0;
        double totalUsage = 0.0;
        for (const auto& disk : disks_) {
            totalUsage += disk.usagePercent;
        }
        return totalUsage / disks_.size();
    } else if (metricPath == "disk.usage_percent.max") {
        if (disks_.empty()) return 0.0;
        double maxUsage = 0.0;
        for (const auto& disk : disks_) {
            if (disk.usagePercent > maxUsage) {
                maxUsage = disk.usagePercent;
            }
        }
        return maxUsage;
    } else if (metricPath == "disk.usage_percent.min") {
        if (disks_.empty()) return 0.0;
        double minUsage = 100.0;
        for (const auto& disk : disks_) {
            if (disk.usagePercent < minUsage) {
                minUsage = disk.usagePercent;
            }
        }
        return minUsage;
    } else if (metricPath == "disk.total_bytes") {
        uint64_t totalBytes = 0;
        for (const auto& disk : disks_) {
            totalBytes += disk.totalBytes;
        }
        return static_cast<double>(totalBytes);
    } else if (metricPath == "disk.used_bytes") {
        uint64_t usedBytes = 0;
        for (const auto& disk : disks_) {
            usedBytes += disk.usedBytes;
        }
        return static_cast<double>(usedBytes);
    } else if (metricPath == "disk.free_bytes") {
        uint64_t freeBytes = 0;
        for (const auto& disk : disks_) {
            freeBytes += disk.freeBytes;
        }
        return static_cast<double>(freeBytes);
    }
    
    // ============ 网络指标 ============
    // 网络聚合指标
    else if (metricPath == "network.rx_rate.total") {
        uint64_t totalRxRate = 0;
        for (const auto& network : networks_) {
            totalRxRate += network.rxRate;
        }
        return static_cast<double>(totalRxRate);
    } else if (metricPath == "network.tx_rate.total") {
        uint64_t totalTxRate = 0;
        for (const auto& network : networks_) {
            totalTxRate += network.txRate;
        }
        return static_cast<double>(totalTxRate);
    } else if (metricPath == "network.rx_bytes.total") {
        uint64_t totalRxBytes = 0;
        for (const auto& network : networks_) {
            totalRxBytes += network.rxBytes;
        }
        return static_cast<double>(totalRxBytes);
    } else if (metricPath == "network.tx_bytes.total") {
        uint64_t totalTxBytes = 0;
        for (const auto& network : networks_) {
            totalTxBytes += network.txBytes;
        }
        return static_cast<double>(totalTxBytes);
    } else if (metricPath == "network.rx_errors.total") {
        uint64_t totalRxErrors = 0;
        for (const auto& network : networks_) {
            totalRxErrors += network.rxErrors;
        }
        return static_cast<double>(totalRxErrors);
    } else if (metricPath == "network.tx_errors.total") {
        uint64_t totalTxErrors = 0;
        for (const auto& network : networks_) {
            totalTxErrors += network.txErrors;
        }
        return static_cast<double>(totalTxErrors);
    }
    
    // ============ GPU指标 ============
    // GPU聚合指标
    else if (metricPath == "gpu.compute_usage.avg") {
        if (gpus_.empty()) return 0.0;
        double totalUsage = 0.0;
        for (const auto& gpu : gpus_) {
            totalUsage += gpu.computeUsage;
        }
        return totalUsage / gpus_.size();
    } else if (metricPath == "gpu.compute_usage.max") {
        if (gpus_.empty()) return 0.0;
        double maxUsage = 0.0;
        for (const auto& gpu : gpus_) {
            if (gpu.computeUsage > maxUsage) {
                maxUsage = gpu.computeUsage;
            }
        }
        return maxUsage;
    } else if (metricPath == "gpu.mem_usage.avg") {
        if (gpus_.empty()) return 0.0;
        double totalUsage = 0.0;
        for (const auto& gpu : gpus_) {
            totalUsage += gpu.memUsage;
        }
        return totalUsage / gpus_.size();
    } else if (metricPath == "gpu.mem_usage.max") {
        if (gpus_.empty()) return 0.0;
        double maxUsage = 0.0;
        for (const auto& gpu : gpus_) {
            if (gpu.memUsage > maxUsage) {
                maxUsage = gpu.memUsage;
            }
        }
        return maxUsage;
    } else if (metricPath == "gpu.temperature.avg") {
        if (gpus_.empty()) return 0.0;
        double totalTemp = 0.0;
        for (const auto& gpu : gpus_) {
            totalTemp += gpu.temperature;
        }
        return totalTemp / gpus_.size();
    } else if (metricPath == "gpu.temperature.max") {
        if (gpus_.empty()) return 0.0;
        double maxTemp = 0.0;
        for (const auto& gpu : gpus_) {
            if (gpu.temperature > maxTemp) {
                maxTemp = gpu.temperature;
            }
        }
        return maxTemp;
    } else if (metricPath == "gpu.power.total") {
        double totalPower = 0.0;
        for (const auto& gpu : gpus_) {
            totalPower += gpu.power;
        }
        return totalPower;
    } else if (metricPath == "gpu.count") {
        return static_cast<double>(gpus_.size());
    } else if (metricPath == "gpu.allocated") {
        return static_cast<double>(gpuAllocated_);
    } else if (metricPath == "gpu.total") {
        return static_cast<double>(gpuNum_);
    }
    
    // ============ 特定设备指标 ============
    // 特定磁盘指标 (格式: disk.{device}.{metric})
    else if (metricPath.find("disk.") == 0 && metricPath.find(".usage_percent") != std::string::npos) {
        // 提取设备名
        size_t start = 5; // "disk."的长度
        size_t end = metricPath.find(".usage_percent");
        if (end != std::string::npos) {
            std::string device = metricPath.substr(start, end - start);
            for (const auto& disk : disks_) {
                if (disk.device == device) {
                    return disk.usagePercent;
                }
            }
        }
        return 0.0; // 设备不存在时返回0
    } else if (metricPath.find("disk.") == 0 && metricPath.find(".total_bytes") != std::string::npos) {
        size_t start = 5;
        size_t end = metricPath.find(".total_bytes");
        if (end != std::string::npos) {
            std::string device = metricPath.substr(start, end - start);
            for (const auto& disk : disks_) {
                if (disk.device == device) {
                    return static_cast<double>(disk.totalBytes);
                }
            }
        }
        return 0.0; // 设备不存在时返回0
    } else if (metricPath.find("disk.") == 0 && metricPath.find(".used_bytes") != std::string::npos) {
        size_t start = 5;
        size_t end = metricPath.find(".used_bytes");
        if (end != std::string::npos) {
            std::string device = metricPath.substr(start, end - start);
            for (const auto& disk : disks_) {
                if (disk.device == device) {
                    return static_cast<double>(disk.usedBytes);
                }
            }
        }
        return 0.0; // 设备不存在时返回0
    } else if (metricPath.find("disk.") == 0 && metricPath.find(".free_bytes") != std::string::npos) {
        size_t start = 5;
        size_t end = metricPath.find(".free_bytes");
        if (end != std::string::npos) {
            std::string device = metricPath.substr(start, end - start);
            for (const auto& disk : disks_) {
                if (disk.device == device) {
                    return static_cast<double>(disk.freeBytes);
                }
            }
        }
        return 0.0; // 设备不存在时返回0
    }
    
    // 特定网络接口指标 (格式: network.{interface}.{metric})
    else if (metricPath.find("network.") == 0 && metricPath.find(".rx_rate") != std::string::npos) {
        size_t start = 8; // "network."的长度
        size_t end = metricPath.find(".rx_rate");
        if (end != std::string::npos) {
            std::string interface = metricPath.substr(start, end - start);
            for (const auto& network : networks_) {
                if (network.interfaceName == interface) {
                    return static_cast<double>(network.rxRate);
                }
            }
        }
        return 0.0; // 接口不存在时返回0
    } else if (metricPath.find("network.") == 0 && metricPath.find(".tx_rate") != std::string::npos) {
        size_t start = 8;
        size_t end = metricPath.find(".tx_rate");
        if (end != std::string::npos) {
            std::string interface = metricPath.substr(start, end - start);
            for (const auto& network : networks_) {
                if (network.interfaceName == interface) {
                    return static_cast<double>(network.txRate);
                }
            }
        }
        return 0.0; // 接口不存在时返回0
    } else if (metricPath.find("network.") == 0 && metricPath.find(".rx_bytes") != std::string::npos) {
        size_t start = 8;
        size_t end = metricPath.find(".rx_bytes");
        if (end != std::string::npos) {
            std::string interface = metricPath.substr(start, end - start);
            for (const auto& network : networks_) {
                if (network.interfaceName == interface) {
                    return static_cast<double>(network.rxBytes);
                }
            }
        }
        return 0.0; // 接口不存在时返回0
    } else if (metricPath.find("network.") == 0 && metricPath.find(".tx_bytes") != std::string::npos) {
        size_t start = 8;
        size_t end = metricPath.find(".tx_bytes");
        if (end != std::string::npos) {
            std::string interface = metricPath.substr(start, end - start);
            for (const auto& network : networks_) {
                if (network.interfaceName == interface) {
                    return static_cast<double>(network.txBytes);
                }
            }
        }
        return 0.0; // 接口不存在时返回0
    }
    
    // 特定GPU指标 (格式: gpu.{index}.{metric})
    else if (metricPath.find("gpu.") == 0 && metricPath.find(".compute_usage") != std::string::npos) {
        size_t start = 4; // "gpu."的长度
        size_t end = metricPath.find(".compute_usage");
        if (end != std::string::npos) {
            try {
                int32_t index = std::stoi(metricPath.substr(start, end - start));
                for (const auto& gpu : gpus_) {
                    if (gpu.index == index) {
                        return gpu.computeUsage;
                    }
                }
            } catch (const std::exception&) {
                // 解析失败，返回0
            }
        }
        return 0.0; // GPU不存在时返回0
    } else if (metricPath.find("gpu.") == 0 && metricPath.find(".mem_usage") != std::string::npos) {
        size_t start = 4;
        size_t end = metricPath.find(".mem_usage");
        if (end != std::string::npos) {
            try {
                int32_t index = std::stoi(metricPath.substr(start, end - start));
                for (const auto& gpu : gpus_) {
                    if (gpu.index == index) {
                        return gpu.memUsage;
                    }
                }
            } catch (const std::exception&) {
                // 解析失败，返回0
            }
        }
        return 0.0; // GPU不存在时返回0
    } else if (metricPath.find("gpu.") == 0 && metricPath.find(".temperature") != std::string::npos) {
        size_t start = 4;
        size_t end = metricPath.find(".temperature");
        if (end != std::string::npos) {
            try {
                int32_t index = std::stoi(metricPath.substr(start, end - start));
                for (const auto& gpu : gpus_) {
                    if (gpu.index == index) {
                        return gpu.temperature;
                    }
                }
            } catch (const std::exception&) {
                // 解析失败，返回0
            }
        }
        return 0.0; // GPU不存在时返回0
    } else if (metricPath.find("gpu.") == 0 && metricPath.find(".power") != std::string::npos) {
        size_t start = 4;
        size_t end = metricPath.find(".power");
        if (end != std::string::npos) {
            try {
                int32_t index = std::stoi(metricPath.substr(start, end - start));
                for (const auto& gpu : gpus_) {
                    if (gpu.index == index) {
                        return gpu.power;
                    }
                }
            } catch (const std::exception&) {
                // 解析失败，返回0
            }
        }
        return 0.0; // GPU不存在时返回0
    }
    
    // ============ 错误处理 ============
    else {
        throw std::invalid_argument("Unknown metric path: " + metricPath);
    }
    
    // 这行代码永远不会执行到，但为了消除编译警告
    return 0.0;
}

} // namespace monitoring::domain
