#ifndef AGENT_CONFIG_HPP
#define AGENT_CONFIG_HPP

#include <string>
#include <cstdint>

namespace monitoring::agent {

/**
 * AgentConfig - Agent配置类
 * 存储Agent运行所需的所有配置信息
 */
class AgentConfig {
public:
    // 服务器配置
    std::string serverHost = "localhost";
    uint16_t serverPort = 18888;

    // 上报间隔（秒）
    int32_t heartbeatInterval = 60;      // 心跳间隔：60秒
    int32_t resourceInterval = 1;        // 资源上报间隔：1秒

    // 节点信息
    int32_t boxId = 1;
    int32_t slotId = 1;
    int32_t cpuId = 1;
    int32_t srioId = 0;
    std::string hostIp = "192.168.10.29";
    std::string hostname = "agent-node-01";
    uint16_t servicePort = 23980;

    // 硬件信息
    std::string boxType = "计算I型";
    std::string boardType = "GPU";
    std::string cpuType = "Intel Core i7";
    std::string osType = "Linux 5.15";
    std::string resourceType = "GPU I";
    std::string cpuArch = "x86_64";

    /**
     * 获取心跳API端点
     */
    std::string getHeartbeatUrl() const {
        return "http://" + serverHost + ":" + std::to_string(serverPort) + "/heartbeat";
    }

    /**
     * 获取资源上报API端点
     */
    std::string getResourceUrl() const {
        return "http://" + serverHost + ":" + std::to_string(serverPort) + "/resource";
    }
};

} // namespace monitoring::agent

#endif // AGENT_CONFIG_HPP
