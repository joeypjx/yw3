#ifndef INFRASTRUCTURE_REPOSITORIES_MOCK_METRIC_REPOSITORY_HPP
#define INFRASTRUCTURE_REPOSITORIES_MOCK_METRIC_REPOSITORY_HPP

#include "../../domain/repositories/i_metric_repository.hpp"
#include <vector>
#include <mutex>

namespace monitoring::infrastructure {

/**
 * MockMetricRepository - 内存模拟指标仓储
 * 用于测试，数据存储在内存中
 * 
 * 特性：
 * - 自动数据清理（保留最近N小时的数据）
 * - 内存使用监控
 * - 批量操作优化
 */
class MockMetricRepository : public domain::IMetricRepository {
public:
    MockMetricRepository();
    ~MockMetricRepository();

    void save(const domain::MetricSnapshot& snapshot) override;
    void saveBatch(const std::vector<domain::MetricSnapshot>& snapshots) override;

    std::vector<domain::MetricSnapshot> findByNodeAndTimeRange(
        const std::string& nodeId,
        uint64_t startTime,
        uint64_t endTime
    ) override;

    std::vector<domain::MetricSnapshot> findRecent(
        const std::string& nodeId,
        int32_t recentSeconds
    ) override;

    std::optional<domain::MetricSnapshot> findLatest(const std::string& nodeId) override;

    int64_t deleteOldData(uint64_t beforeTime) override;

    // 新增方法
    size_t getDataCount() const;
    size_t getMemoryUsageMB() const;
    void setRetentionHours(int32_t hours);
    void enableAutoCleanup(bool enable);

private:
    std::vector<domain::MetricSnapshot> snapshots_;
    std::mutex mutex_;
    
    // 配置参数
    int32_t retentionHours_;     // 数据保留时间（小时）
    bool autoCleanupEnabled_;     // 是否启用自动清理
    size_t maxDataCount_;         // 最大数据条数限制
    
    // 内部方法
    void autoCleanup();
    void logMemoryUsage();
};

} // namespace monitoring::infrastructure

#endif // INFRASTRUCTURE_REPOSITORIES_MOCK_METRIC_REPOSITORY_HPP
