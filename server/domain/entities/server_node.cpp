#include "server_node.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace monitoring::domain {

ServerNode::ServerNode(
    const std::string& nodeId,
    const std::string& hostname,
    const std::string& ipAddress,
    const HardwareInfo& hwInfo
)
    : nodeId_(nodeId)
    , hostname_(hostname)
    , ipAddress_(ipAddress)
    , hardwareInfo_(hwInfo)
    , status_(Status::UNKNOWN)
    , createdAt_(getCurrentTimestamp())
    , lastSeenAt_(getCurrentTimestamp())
{
}

void ServerNode::updateLastSeen() {
    lastSeenAt_ = getCurrentTimestamp();
}

void ServerNode::updateStatus(Status newStatus) {
    status_ = newStatus;
}

bool ServerNode::isOnline(uint64_t timeoutSeconds) const {
    uint64_t now = getCurrentTimestamp();
    return (now - lastSeenAt_) <= timeoutSeconds;
}

std::string ServerNode::generateNodeId(const std::string& ipAddress, const HardwareInfo& hwInfo) {
    // 生成格式: ip_boxId_slotId_cpuId
    std::ostringstream oss;
    oss << ipAddress << "_"
        << hwInfo.boxId << "_"
        << hwInfo.slotId << "_"
        << hwInfo.cpuId;
    return oss.str();
}

uint64_t ServerNode::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

// 辅助函数实现
std::string statusToString(ServerNode::Status status) {
    switch (status) {
        case ServerNode::Status::UNKNOWN:  return "UNKNOWN";
        case ServerNode::Status::HEALTHY:  return "HEALTHY";
        case ServerNode::Status::WARNING:  return "WARNING";
        case ServerNode::Status::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

ServerNode::Status stringToStatus(const std::string& str) {
    if (str == "HEALTHY")  return ServerNode::Status::HEALTHY;
    if (str == "WARNING")  return ServerNode::Status::WARNING;
    if (str == "CRITICAL") return ServerNode::Status::CRITICAL;
    return ServerNode::Status::UNKNOWN;
}

} // namespace monitoring::domain
