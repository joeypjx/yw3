#include "mock_metric_repository.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include "../../logging_utils.hpp"

namespace monitoring::infrastructure {

MockMetricRepository::MockMetricRepository() 
    : retentionHours_(24),      // 默认保留24小时
      autoCleanupEnabled_(true), // 默认启用自动清理
      maxDataCount_(100000)     // 默认最大10万条记录
{
    monitoring::utils::Logger::info("📊 [MockMetricRepository] Initialized with retention: " + 
                                   std::to_string(retentionHours_) + "h, max records: " + 
                                   std::to_string(maxDataCount_));
}

MockMetricRepository::~MockMetricRepository() {
    std::lock_guard<std::mutex> lock(mutex_);
    monitoring::utils::Logger::info("📊 [MockMetricRepository] Destroyed with " + 
                                   std::to_string(snapshots_.size()) + " records");
}

void MockMetricRepository::save(const domain::MetricSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 检查数据量限制
    if (snapshots_.size() >= maxDataCount_) {
        autoCleanup();
    }
    
    snapshots_.push_back(snapshot);
    
    // 打印保存的指标信息
    std::ostringstream oss;
    oss << "📊 [MockMetricRepository] Saved metrics: Node=" << snapshot.getNodeId()
        << ", CPU=" << std::fixed << std::setprecision(1) << snapshot.getCpu().usagePercent << "%"
        << ", Memory=" << snapshot.getMemory().usagePercent << "%"
        << ", Timestamp=" << snapshot.getTimestamp()
        << ", Total records=" << snapshots_.size();
    monitoring::utils::Logger::info(oss.str());
    
    // 定期记录内存使用情况
    if (snapshots_.size() % 1000 == 0) {
        logMemoryUsage();
    }
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

    if (count > 0) {
        monitoring::utils::Logger::info("📊 [MockMetricRepository] Cleaned " + 
                                      std::to_string(count) + " old records, remaining: " + 
                                      std::to_string(snapshots_.size()));
    }

    return count;
}

size_t MockMetricRepository::getDataCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex_));
    return snapshots_.size();
}

size_t MockMetricRepository::getMemoryUsageMB() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex_));
    
    // 估算内存使用量
    size_t estimatedSize = 0;
    for (const auto& snapshot : snapshots_) {
        // 基础字段
        estimatedSize += sizeof(snapshot);
        
        // 向量大小估算
        estimatedSize += snapshot.getDisks().size() * sizeof(domain::DiskMetrics);
        estimatedSize += snapshot.getNetworks().size() * sizeof(domain::NetworkMetrics);
        estimatedSize += snapshot.getGpus().size() * sizeof(domain::GpuMetrics);
        estimatedSize += snapshot.getComponents().size() * sizeof(domain::ComponentMetrics);
    }
    
    return estimatedSize / (1024 * 1024); // 转换为MB
}

void MockMetricRepository::setRetentionHours(int32_t hours) {
    std::lock_guard<std::mutex> lock(mutex_);
    retentionHours_ = hours;
    monitoring::utils::Logger::info("📊 [MockMetricRepository] Retention set to " + 
                                   std::to_string(hours) + " hours");
}

void MockMetricRepository::enableAutoCleanup(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    autoCleanupEnabled_ = enable;
    monitoring::utils::Logger::info("📊 [MockMetricRepository] Auto cleanup " + 
                                   std::string(enable ? "enabled" : "disabled"));
}

void MockMetricRepository::autoCleanup() {
    if (!autoCleanupEnabled_) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    uint64_t nowEpoch = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();
    
    uint64_t cutoffTime = nowEpoch - (retentionHours_ * 3600);
    
    int64_t deletedCount = deleteOldData(cutoffTime);
    
    if (deletedCount > 0) {
        monitoring::utils::Logger::info("📊 [MockMetricRepository] Auto cleanup removed " + 
                                      std::to_string(deletedCount) + " records");
    }
}

void MockMetricRepository::logMemoryUsage() {
    size_t count = snapshots_.size();
    size_t memoryMB = getMemoryUsageMB();
    
    std::ostringstream oss;
    oss << "📊 [MockMetricRepository] Memory usage: " << count 
        << " records, ~" << memoryMB << "MB";
    monitoring::utils::Logger::info(oss.str());
}

} // namespace monitoring::infrastructure
