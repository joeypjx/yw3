#ifndef DOMAIN_REPOSITORIES_I_ALERT_EVENT_REPOSITORY_HPP
#define DOMAIN_REPOSITORIES_I_ALERT_EVENT_REPOSITORY_HPP

#include <string>
#include <vector>
#include <optional>
#include "../entities/alert_event.hpp"

namespace monitoring::domain {

/**
 * IAlertEventRepository - 告警事件仓储接口
 * 定义如何持久化和检索 AlertEvent
 */
class IAlertEventRepository {
public:
    virtual ~IAlertEventRepository() = default;

    /**
     * 保存或更新告警事件
     * @param event 告警事件
     */
    virtual void save(const AlertEvent& event) = 0;

    /**
     * 根据事件ID查找事件
     * @param eventId 事件ID
     * @return 如果找到返回事件，否则返回 std::nullopt
     */
    virtual std::optional<AlertEvent> findById(int64_t eventId) = 0;

    /**
     * 查找某个节点的所有告警事件
     * @param nodeId 节点ID
     * @return 告警事件列表
     */
    virtual std::vector<AlertEvent> findByNode(const std::string& nodeId) = 0;

    /**
     * 查找某个规则触发的所有告警事件
     * @param ruleId 规则ID
     * @return 告警事件列表
     */
    virtual std::vector<AlertEvent> findByRule(int32_t ruleId) = 0;

    /**
     * 查找某个节点在特定规则下的活跃告警
     * 用于判断是否已经存在相同的告警，避免重复触发
     *
     * @param ruleId 规则ID
     * @param nodeId 节点ID
     * @return 如果存在活跃告警返回该事件，否则返回 std::nullopt
     */
    virtual std::optional<AlertEvent> findActiveEvent(
        int32_t ruleId,
        const std::string& nodeId
    ) = 0;

    /**
     * 查找所有告警事件
     * @return 所有告警事件列表
     */
    virtual std::vector<AlertEvent> findAll() = 0;

    /**
     * 查找所有活跃的告警事件（FIRING 或 ACKNOWLEDGED）
     * @return 活跃的告警事件列表
     */
    virtual std::vector<AlertEvent> findAllActive() = 0;

    /**
     * 查找指定状态的告警事件
     * @param status 告警状态
     * @return 匹配的告警事件列表
     */
    virtual std::vector<AlertEvent> findByStatus(AlertEvent::Status status) = 0;

    /**
     * 查找时间范围内的告警事件
     * @param startTime 开始时间戳（秒）
     * @param endTime 结束时间戳（秒）
     * @return 告警事件列表
     */
    virtual std::vector<AlertEvent> findByTimeRange(
        uint64_t startTime,
        uint64_t endTime
    ) = 0;

    /**
     * 删除告警事件
     * @param eventId 事件ID
     * @return 是否删除成功
     */
    virtual bool remove(int64_t eventId) = 0;
};

} // namespace monitoring::domain

#endif // DOMAIN_REPOSITORIES_I_ALERT_EVENT_REPOSITORY_HPP
