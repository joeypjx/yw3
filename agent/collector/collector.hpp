#ifndef AGENT_COLLECTOR_HPP
#define AGENT_COLLECTOR_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <random>

namespace monitoring::agent {

/**
 * DataCollector - 数据采集器
 * 负责生成模拟的系统资源数据
 */
class DataCollector {
public:
    DataCollector();

    /**
     * 生成心跳数据（JSON格式）
     */
    std::string generateHeartbeatData(
        int32_t boxId,
        int32_t slotId,
        int32_t cpuId,
        int32_t srioId,
        const std::string& hostIp,
        const std::string& hostname,
        uint16_t servicePort,
        const std::string& boxType,
        const std::string& boardType,
        const std::string& cpuType,
        const std::string& osType,
        const std::string& resourceType,
        const std::string& cpuArch
    );

    /**
     * 生成资源数据（JSON格式）
     */
    std::string generateResourceData(const std::string& hostIp);

private:
    std::random_device rd_;
    std::mt19937 gen_;

    // 用于生成随机的CPU使用率 (0-100)
    std::uniform_real_distribution<double> cpuUsageDist_;
    // 用于生成随机的内存使用率 (30-90)
    std::uniform_real_distribution<double> memUsageDist_;
    // 用于生成随机的温度 (40-80)
    std::uniform_real_distribution<double> tempDist_;

    // 辅助方法：转义JSON字符串
    std::string escapeJson(const std::string& str);
};

} // namespace monitoring::agent

#endif // AGENT_COLLECTOR_HPP
