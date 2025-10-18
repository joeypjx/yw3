#ifndef DOMAIN_REPOSITORIES_I_METRIC_REPOSITORY_HPP
#define DOMAIN_REPOSITORIES_I_METRIC_REPOSITORY_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "../entities/metric_snapshot.hpp"

namespace monitoring::domain {

/**
 * IMetricRepository - 指标数据仓储接口
 * 定义如何持久化和检索时间序列的 MetricSnapshot 数据
 * 通常由时序数据库（如 InfluxDB）实现
 */
class IMetricRepository {
public:
    virtual ~IMetricRepository() = default;

    /**
     * 保存指标快照
     * @param snapshot 指标快照
     */
    virtual void save(const MetricSnapshot& snapshot) = 0;

    /**
     * 批量保存指标快照
     * @param snapshots 指标快照列表
     */
    virtual void saveBatch(const std::vector<MetricSnapshot>& snapshots) = 0;

    /**
     * 查询某个节点在指定时间范围内的指标
     * @param nodeId 节点ID
     * @param startTime 起始时间戳（秒）
     * @param endTime 结束时间戳（秒）
     * @return 指标快照列表，按时间戳升序排序
     */
    virtual std::vector<MetricSnapshot> findByNodeAndTimeRange(
        const std::string& nodeId,
        uint64_t startTime,
        uint64_t endTime
    ) = 0;

    /**
     * 查询某个节点最近N秒的指标
     * @param nodeId 节点ID
     * @param recentSeconds 最近多少秒
     * @return 指标快照列表，按时间戳升序排序
     */
    virtual std::vector<MetricSnapshot> findRecent(
        const std::string& nodeId,
        int32_t recentSeconds
    ) = 0;

    /**
     * 查询某个节点的最新一条指标
     * @param nodeId 节点ID
     * @return 最新的指标快照，如果不存在返回 std::nullopt
     */
    virtual std::optional<MetricSnapshot> findLatest(const std::string& nodeId) = 0;

    /**
     * 删除指定时间之前的旧数据（数据清理）
     * @param beforeTime 时间戳（秒）
     * @return 删除的记录数
     */
    virtual int64_t deleteOldData(uint64_t beforeTime) = 0;
};

} // namespace monitoring::domain

#endif // DOMAIN_REPOSITORIES_I_METRIC_REPOSITORY_HPP
