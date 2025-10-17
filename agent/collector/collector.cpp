#include "collector.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace monitoring::agent {

DataCollector::DataCollector()
    : gen_(rd_())
    , cpuUsageDist_(0.0, 100.0)
    , memUsageDist_(30.0, 90.0)
    , tempDist_(40.0, 80.0)
{
}

std::string DataCollector::generateHeartbeatData(
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
) {
    std::ostringstream json;
    json << std::fixed << std::setprecision(1);

    json << "{\n";
    json << "  \"api_version\": 1,\n";
    json << "  \"data\": {\n";
    json << "    \"box_id\": " << boxId << ",\n";
    json << "    \"slot_id\": " << slotId << ",\n";
    json << "    \"cpu_id\": " << cpuId << ",\n";
    json << "    \"srio_id\": " << srioId << ",\n";
    json << "    \"host_ip\": \"" << hostIp << "\",\n";
    json << "    \"hostname\": \"" << hostname << "\",\n";
    json << "    \"service_port\": " << servicePort << ",\n";
    json << "    \"box_type\": \"" << boxType << "\",\n";
    json << "    \"board_type\": \"" << boardType << "\",\n";
    json << "    \"cpu_type\": \"" << cpuType << "\",\n";
    json << "    \"os_type\": \"" << osType << "\",\n";
    json << "    \"resource_type\": \"" << resourceType << "\",\n";
    json << "    \"cpu_arch\": \"" << cpuArch << "\",\n";
    json << "    \"gpu\": [\n";
    json << "      {\"index\": 0, \"name\": \"NVIDIA GeForce RTX 3080\"},\n";
    json << "      {\"index\": 1, \"name\": \"NVIDIA GeForce RTX 3080\"}\n";
    json << "    ]\n";
    json << "  }\n";
    json << "}\n";

    return json.str();
}

std::string DataCollector::generateResourceData(const std::string& hostIp) {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);

    // 生成随机数据
    double cpuUsage = cpuUsageDist_(gen_);
    double memUsagePercent = memUsageDist_(gen_);
    double cpuTemp = tempDist_(gen_);
    double gpu0Usage = cpuUsageDist_(gen_);
    double gpu1Usage = cpuUsageDist_(gen_);

    // 模拟内存数据（16GB总内存）
    uint64_t totalMem = 17179869184ULL;  // 16GB
    uint64_t usedMem = static_cast<uint64_t>(totalMem * memUsagePercent / 100.0);
    uint64_t freeMem = totalMem - usedMem;

    // 模拟磁盘数据（500GB总容量）
    uint64_t totalDisk = 536870912000ULL;  // 500GB
    uint64_t usedDisk = totalDisk / 2;     // 50%使用
    uint64_t freeDisk = totalDisk - usedDisk;

    json << "{\n";
    json << "  \"api_version\": 1,\n";
    json << "  \"data\": {\n";
    json << "    \"host_ip\": \"" << hostIp << "\",\n";
    json << "    \"resource\": {\n";

    // CPU
    json << "      \"cpu\": {\n";
    json << "        \"usage_percent\": " << cpuUsage << ",\n";
    json << "        \"load_avg_1m\": " << (cpuUsage / 100.0 * 4.0) << ",\n";
    json << "        \"load_avg_5m\": " << (cpuUsage / 100.0 * 3.5) << ",\n";
    json << "        \"load_avg_15m\": " << (cpuUsage / 100.0 * 3.0) << ",\n";
    json << "        \"core_count\": 16,\n";
    json << "        \"core_allocated\": 8,\n";
    json << "        \"temperature\": " << cpuTemp << ",\n";
    json << "        \"voltage\": 1.1,\n";
    json << "        \"current\": 2.3,\n";
    json << "        \"power\": " << (25.0 + cpuUsage / 10.0) << "\n";
    json << "      },\n";

    // Memory
    json << "      \"memory\": {\n";
    json << "        \"total\": " << totalMem << ",\n";
    json << "        \"used\": " << usedMem << ",\n";
    json << "        \"free\": " << freeMem << ",\n";
    json << "        \"usage_percent\": " << memUsagePercent << "\n";
    json << "      },\n";

    // Network
    json << "      \"network\": [\n";
    json << "        {\n";
    json << "          \"interface\": \"eth0\",\n";
    json << "          \"rx_bytes\": 123456789,\n";
    json << "          \"tx_bytes\": 987654321,\n";
    json << "          \"rx_packets\": 10000,\n";
    json << "          \"tx_packets\": 9000,\n";
    json << "          \"rx_errors\": 0,\n";
    json << "          \"tx_errors\": 0,\n";
    json << "          \"rx_rate\": 2048,\n";
    json << "          \"tx_rate\": 4096\n";
    json << "        }\n";
    json << "      ],\n";

    // Disk
    json << "      \"disk\": [\n";
    json << "        {\n";
    json << "          \"device\": \"/dev/sda1\",\n";
    json << "          \"mount_point\": \"/\",\n";
    json << "          \"total\": " << totalDisk << ",\n";
    json << "          \"used\": " << usedDisk << ",\n";
    json << "          \"free\": " << freeDisk << ",\n";
    json << "          \"usage_percent\": 50.0\n";
    json << "        }\n";
    json << "      ],\n";

    // GPU
    json << "      \"gpu\": [\n";
    json << "        {\n";
    json << "          \"index\": 0,\n";
    json << "          \"name\": \"NVIDIA RTX 3080\",\n";
    json << "          \"compute_usage\": " << gpu0Usage << ",\n";
    json << "          \"mem_usage\": " << (gpu0Usage * 0.8) << ",\n";
    json << "          \"mem_used\": 3221225472,\n";
    json << "          \"mem_total\": 10737418240,\n";
    json << "          \"temperature\": " << (tempDist_(gen_)) << ",\n";
    json << "          \"power\": " << (120.0 + gpu0Usage / 5.0) << "\n";
    json << "        },\n";
    json << "        {\n";
    json << "          \"index\": 1,\n";
    json << "          \"name\": \"NVIDIA RTX 3080\",\n";
    json << "          \"compute_usage\": " << gpu1Usage << ",\n";
    json << "          \"mem_usage\": " << (gpu1Usage * 0.8) << ",\n";
    json << "          \"mem_used\": 2147483648,\n";
    json << "          \"mem_total\": 10737418240,\n";
    json << "          \"temperature\": " << (tempDist_(gen_)) << ",\n";
    json << "          \"power\": " << (120.0 + gpu1Usage / 5.0) << "\n";
    json << "        }\n";
    json << "      ],\n";

    json << "      \"gpu_allocated\": 1,\n";
    json << "      \"gpu_num\": 2\n";

    json << "    },\n";

    // Component (可选，这里添加一个示例容器)
    json << "    \"component\": [\n";
    json << "      {\n";
    json << "        \"instance_id\": \"container-001\",\n";
    json << "        \"uuid\": \"uuid-abc-123\",\n";
    json << "        \"index\": 0,\n";
    json << "        \"config\": {\n";
    json << "          \"name\": \"app-container\",\n";
    json << "          \"id\": \"cont-id-001\"\n";
    json << "        },\n";
    json << "        \"state\": \"RUNNING\",\n";
    json << "        \"resource\": {\n";
    json << "          \"cpu\": {\"load\": " << (cpuUsage / 8.0) << "},\n";
    json << "          \"memory\": {\"mem_used\": 134217728, \"mem_limit\": 268435456},\n";
    json << "          \"network\": {\"tx\": 10240, \"rx\": 20480}\n";
    json << "        }\n";
    json << "      }\n";
    json << "    ]\n";

    json << "  }\n";
    json << "}\n";

    return json.str();
}

std::string DataCollector::escapeJson(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:   oss << c; break;
        }
    }
    return oss.str();
}

} // namespace monitoring::agent
