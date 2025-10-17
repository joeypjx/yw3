#include "mock_metric_repository.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include "../../logging_utils.hpp"

namespace monitoring::infrastructure {

void MockMetricRepository::save(const domain::MetricSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshots_.push_back(snapshot);
    
    // 打印保存的指标信息
    std::ostringstream oss;
    oss << "📊 [MockMetricRepository] Saved metrics: Node=" << snapshot.getNodeId()
        << ", CPU=" << std::fixed << std::setprecision(1) << snapshot.getCpu().usagePercent << "%"
        << ", Memory=" << snapshot.getMemory().usagePercent << "%"
        << ", Timestamp=" << snapshot.getTimestamp();
    monitoring::utils::Logger::info(oss.str());
}

void MockMetricRepository::saveBatch(const std::vector<domain::MetricSnapshot>& snapshots) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshots_.insert(snapshots_.end(), snapshots.begin(), snapshots.end());
}

std::vector<domain::MetricSnapshot> MockMetricRepository::findByNodeAndTimeRange(
    const std::string& nodeId,
    uint64_t startTime,
    uint64_t endTime
) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::MetricSnapshot> result;

    for (const auto& snapshot : snapshots_) {
        if (snapshot.getNodeId() == nodeId &&
            snapshot.getTimestamp() >= startTime &&
            snapshot.getTimestamp() <= endTime) {
            result.push_back(snapshot);
        }
    }

    // 按时间戳排序
    std::sort(result.begin(), result.end(),
        [](const domain::MetricSnapshot& a, const domain::MetricSnapshot& b) {
            return a.getTimestamp() < b.getTimestamp();
        });

    return result;
}

std::vector<domain::MetricSnapshot> MockMetricRepository::findRecent(
    const std::string& nodeId,
    int32_t recentSeconds
) {
    auto now = std::chrono::system_clock::now();
    uint64_t nowEpoch = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();

    uint64_t startTime = nowEpoch - recentSeconds;
    return findByNodeAndTimeRange(nodeId, startTime, nowEpoch);
}

std::optional<domain::MetricSnapshot> MockMetricRepository::findLatest(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::optional<domain::MetricSnapshot> latest;
    uint64_t maxTimestamp = 0;

    for (const auto& snapshot : snapshots_) {
        if (snapshot.getNodeId() == nodeId && snapshot.getTimestamp() > maxTimestamp) {
            latest = snapshot;
            maxTimestamp = snapshot.getTimestamp();
        }
    }

    return latest;
}

int64_t MockMetricRepository::deleteOldData(uint64_t beforeTime) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::remove_if(snapshots_.begin(), snapshots_.end(),
        [beforeTime](const domain::MetricSnapshot& s) {
            return s.getTimestamp() < beforeTime;
        });

    int64_t count = std::distance(it, snapshots_.end());
    snapshots_.erase(it, snapshots_.end());

    return count;
}

} // namespace monitoring::infrastructure
