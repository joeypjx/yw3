#ifndef COMMON_ID_GENERATOR_HPP
#define COMMON_ID_GENERATOR_HPP

#include <atomic>
#include <cstdint>
#include <chrono>
#include <random>

namespace monitoring::common {

/**
 * IdGenerator - 线程安全的ID生成器
 *
 * 提供多种ID生成策略:
 * 1. 原子递增ID (线程安全)
 * 2. 时间戳+序列号组合ID
 */
class IdGenerator {
public:
    /**
     * 获取单例实例
     */
    static IdGenerator& getInstance() {
        static IdGenerator instance;
        return instance;
    }

    /**
     * 生成下一个告警事件ID (线程安全的递增ID)
     *
     * @return 唯一的事件ID
     */
    int64_t nextAlertEventId() {
        return nextEventId_.fetch_add(1, std::memory_order_relaxed);
    }

    /**
     * 生成下一个告警规则ID (线程安全的递增ID)
     *
     * @return 唯一的规则ID
     */
    int32_t nextAlertRuleId() {
        return static_cast<int32_t>(nextRuleId_.fetch_add(1, std::memory_order_relaxed));
    }

    /**
     * 生成基于时间戳的ID (适合分布式环境)
     *
     * 格式: 高32位为时间戳(秒), 低32位为递增序列号
     * 这样即使进程重启,ID也不会重复
     *
     * @return 唯一的64位ID
     */
    int64_t nextTimestampBasedId() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()
        ).count();

        uint32_t sequence = sequenceCounter_.fetch_add(1, std::memory_order_relaxed);

        // 高32位: 时间戳, 低32位: 序列号
        int64_t id = (static_cast<int64_t>(timestamp) << 32) | sequence;
        return id;
    }

    /**
     * 重置计数器 (通常用于测试)
     * 注意: 生产环境不应该调用此方法
     */
    void reset() {
        nextEventId_.store(1, std::memory_order_relaxed);
        nextRuleId_.store(1, std::memory_order_relaxed);
        sequenceCounter_.store(0, std::memory_order_relaxed);
    }

    /**
     * 设置起始ID (用于从持久化存储恢复)
     *
     * @param startEventId 告警事件起始ID
     * @param startRuleId 告警规则起始ID
     */
    void initialize(int64_t startEventId, int32_t startRuleId) {
        nextEventId_.store(startEventId, std::memory_order_relaxed);
        nextRuleId_.store(startRuleId, std::memory_order_relaxed);
    }

private:
    IdGenerator()
        : nextEventId_(1)
        , nextRuleId_(1)
        , sequenceCounter_(0)
    {}

    // 禁用拷贝和赋值
    IdGenerator(const IdGenerator&) = delete;
    IdGenerator& operator=(const IdGenerator&) = delete;

    std::atomic<int64_t> nextEventId_;      // 告警事件ID计数器
    std::atomic<int32_t> nextRuleId_;       // 告警规则ID计数器
    std::atomic<uint32_t> sequenceCounter_; // 时间戳ID的序列号计数器
};

} // namespace monitoring::common

#endif // COMMON_ID_GENERATOR_HPP
