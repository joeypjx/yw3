#ifndef INFRASTRUCTURE_REPOSITORIES_MOCK_METRIC_REPOSITORY_HPP
#define INFRASTRUCTURE_REPOSITORIES_MOCK_METRIC_REPOSITORY_HPP

#include "../../domain/repositories/i_metric_repository.hpp"
#include <vector>
#include <mutex>

namespace monitoring::infrastructure {

/**
 * MockMetricRepository - 内存模拟指标仓储
 * 用于测试，数据存储在内存中
 */
class MockMetricRepository : public domain::IMetricRepository {
public:
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

private:
    std::vector<domain::MetricSnapshot> snapshots_;
    std::mutex mutex_;
};

} // namespace monitoring::infrastructure

#endif // INFRASTRUCTURE_REPOSITORIES_MOCK_METRIC_REPOSITORY_HPP
