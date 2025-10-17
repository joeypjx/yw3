#include "mock_node_repository.hpp"
#include <algorithm>
#include <iostream>
#include "../../logging_utils.hpp"

namespace monitoring::infrastructure {

void MockNodeRepository::save(const domain::ServerNode& node) {
    std::lock_guard<std::mutex> lock(mutex_);
    nodes_[node.getNodeId()] = std::make_unique<domain::ServerNode>(node);
    
    // 打印保存的节点信息
    monitoring::utils::Logger::info("💾 [MockNodeRepository] Saved node: " + node.getNodeId() + 
                                   " (IP: " + node.getIpAddress() + 
                                   ", Hostname: " + node.getHostname() + 
                                   ", Status: " + domain::statusToString(node.getStatus()) + ")");
}

std::optional<domain::ServerNode> MockNodeRepository::findById(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        return *(it->second);
    }
    return std::nullopt;
}

std::vector<domain::ServerNode> MockNodeRepository::findByIpAddress(const std::string& ipAddress) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::ServerNode> result;
    for (const auto& pair : nodes_) {
        if (pair.second->getIpAddress() == ipAddress) {
            result.push_back(*(pair.second));
        }
    }
    return result;
}

std::vector<domain::ServerNode> MockNodeRepository::findAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::ServerNode> result;
    for (const auto& pair : nodes_) {
        result.push_back(*(pair.second));
    }
    return result;
}

std::vector<domain::ServerNode> MockNodeRepository::findByStatus(domain::ServerNode::Status status) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::ServerNode> result;
    for (const auto& pair : nodes_) {
        if (pair.second->getStatus() == status) {
            result.push_back(*(pair.second));
        }
    }
    return result;
}

bool MockNodeRepository::remove(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    return nodes_.erase(nodeId) > 0;
}

bool MockNodeRepository::exists(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    return nodes_.find(nodeId) != nodes_.end();
}

} // namespace monitoring::infrastructure
