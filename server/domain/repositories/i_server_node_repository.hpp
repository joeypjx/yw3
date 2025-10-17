#ifndef DOMAIN_REPOSITORIES_I_SERVER_NODE_REPOSITORY_HPP
#define DOMAIN_REPOSITORIES_I_SERVER_NODE_REPOSITORY_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "../entities/server_node.hpp"

namespace monitoring::domain {

/**
 * IServerNodeRepository - 服务器节点仓储接口
 * 定义如何持久化和检索 ServerNode 的契约
 * 这是一个纯抽象接口，遵循依赖倒置原则
 */
class IServerNodeRepository {
public:
    virtual ~IServerNodeRepository() = default;

    /**
     * 保存或更新节点
     * @param node 要保存的节点
     */
    virtual void save(const ServerNode& node) = 0;

    /**
     * 根据节点ID查找节点
     * @param nodeId 节点ID
     * @return 如果找到返回节点，否则返回 std::nullopt
     */
    virtual std::optional<ServerNode> findById(const std::string& nodeId) = 0;

    /**
     * 根据IP地址查找节点
     * @param ipAddress IP地址
     * @return 匹配的节点列表
     */
    virtual std::vector<ServerNode> findByIpAddress(const std::string& ipAddress) = 0;

    /**
     * 查找所有节点
     * @return 所有节点的列表
     */
    virtual std::vector<ServerNode> findAll() = 0;

    /**
     * 查找特定状态的节点
     * @param status 节点状态
     * @return 匹配的节点列表
     */
    virtual std::vector<ServerNode> findByStatus(ServerNode::Status status) = 0;

    /**
     * 删除节点
     * @param nodeId 节点ID
     * @return 是否删除成功
     */
    virtual bool remove(const std::string& nodeId) = 0;

    /**
     * 检查节点是否存在
     * @param nodeId 节点ID
     * @return 存在返回true，否则返回false
     */
    virtual bool exists(const std::string& nodeId) = 0;
};

} // namespace monitoring::domain

#endif // DOMAIN_REPOSITORIES_I_SERVER_NODE_REPOSITORY_HPP
