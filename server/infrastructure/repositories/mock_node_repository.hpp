#ifndef INFRASTRUCTURE_REPOSITORIES_MOCK_NODE_REPOSITORY_HPP
#define INFRASTRUCTURE_REPOSITORIES_MOCK_NODE_REPOSITORY_HPP

#include "../../domain/repositories/i_server_node_repository.hpp"
#include <map>
#include <mutex>

namespace monitoring::infrastructure {

/**
 * MockNodeRepository - 内存模拟仓储
 * 用于测试，数据存储在内存中
 */
class MockNodeRepository : public domain::IServerNodeRepository {
public:
    void save(const domain::ServerNode& node) override;
    std::optional<domain::ServerNode> findById(const std::string& nodeId) override;
    std::vector<domain::ServerNode> findByIpAddress(const std::string& ipAddress) override;
    std::vector<domain::ServerNode> findAll() override;
    std::vector<domain::ServerNode> findByStatus(domain::ServerNode::Status status) override;
    bool remove(const std::string& nodeId) override;
    bool exists(const std::string& nodeId) override;

private:
    std::map<std::string, std::unique_ptr<domain::ServerNode>> nodes_;
    std::mutex mutex_;
};

} // namespace monitoring::infrastructure

#endif // INFRASTRUCTURE_REPOSITORIES_MOCK_NODE_REPOSITORY_HPP
