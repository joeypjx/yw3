#include "config/config.hpp"
#include "collector/collector.hpp"
#include "reporter/reporter.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

// 全局标志，用于优雅退出
std::atomic<bool> g_running(true);

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_running = false;
}

int main(int argc, char* argv[]) {
    using namespace monitoring::agent;

    // 注册信号处理器（Ctrl+C优雅退出）
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "==================================================" << std::endl;
    std::cout << "  Monitoring Agent Starting..." << std::endl;
    std::cout << "==================================================" << std::endl;

    // 1. 加载配置
    AgentConfig config;

    // 可以从命令行参数覆盖配置
    if (argc >= 2) {
        config.serverHost = argv[1];
    }
    if (argc >= 3) {
        config.serverPort = static_cast<uint16_t>(std::stoi(argv[2]));
    }

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Server: " << config.serverHost << ":" << config.serverPort << std::endl;
    std::cout << "  Node IP: " << config.hostIp << std::endl;
    std::cout << "  Heartbeat Interval: " << config.heartbeatInterval << "s" << std::endl;
    std::cout << "  Resource Interval: " << config.resourceInterval << "s" << std::endl;
    std::cout << std::endl;

    // 2. 创建组件
    DataCollector collector;
    Reporter reporter;

    // 3. 首次发送心跳（注册节点）
    std::cout << "Sending initial heartbeat..." << std::endl;
    std::string heartbeatData = collector.generateHeartbeatData(
        config.boxId,
        config.slotId,
        config.cpuId,
        config.srioId,
        config.hostIp,
        config.hostname,
        config.servicePort,
        config.boxType,
        config.boardType,
        config.cpuType,
        config.osType,
        config.resourceType,
        config.cpuArch
    );

    if (!reporter.sendPost(config.getHeartbeatUrl(), heartbeatData)) {
        std::cerr << "Failed to send initial heartbeat: " << reporter.getLastError() << std::endl;
        std::cerr << "Warning: Node may not be registered. Continuing anyway..." << std::endl;
    }

    // 4. 主循环：定期上报数据
    std::cout << "\nAgent is running. Press Ctrl+C to stop.\n" << std::endl;

    auto lastHeartbeatTime = std::chrono::steady_clock::now();
    auto lastResourceTime = std::chrono::steady_clock::now();

    int heartbeatCount = 0;
    int resourceCount = 0;

    while (g_running) {
        auto now = std::chrono::steady_clock::now();

        // 检查是否需要发送心跳
        auto heartbeatElapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastHeartbeatTime
        ).count();

        if (heartbeatElapsed >= config.heartbeatInterval) {
            std::cout << "[" << ++heartbeatCount << "] Sending heartbeat..." << std::endl;

            std::string heartbeat = collector.generateHeartbeatData(
                config.boxId, config.slotId, config.cpuId, config.srioId,
                config.hostIp, config.hostname, config.servicePort,
                config.boxType, config.boardType, config.cpuType,
                config.osType, config.resourceType, config.cpuArch
            );

            if (reporter.sendPost(config.getHeartbeatUrl(), heartbeat)) {
                std::cout << "  ✓ Heartbeat sent successfully" << std::endl;
            } else {
                std::cerr << "  ✗ Failed to send heartbeat: "
                          << reporter.getLastError() << std::endl;
            }

            lastHeartbeatTime = now;
        }

        // 检查是否需要发送资源数据
        auto resourceElapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastResourceTime
        ).count();

        if (resourceElapsed >= config.resourceInterval) {
            std::cout << "[" << ++resourceCount << "] Sending resource data..." << std::endl;

            std::string resource = collector.generateResourceData(config.hostIp);

            if (reporter.sendPost(config.getResourceUrl(), resource)) {
                std::cout << "  ✓ Resource data sent successfully" << std::endl;
            } else {
                std::cerr << "  ✗ Failed to send resource data: "
                          << reporter.getLastError() << std::endl;
            }

            lastResourceTime = now;
        }

        // 休眠1秒，减少CPU占用
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\n==================================================" << std::endl;
    std::cout << "  Agent stopped gracefully." << std::endl;
    std::cout << "  Total heartbeats sent: " << heartbeatCount << std::endl;
    std::cout << "  Total resource reports sent: " << resourceCount << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
