#ifndef APPLICATION_DTO_DTO_CONVERTER_HPP
#define APPLICATION_DTO_DTO_CONVERTER_HPP

#include "heartbeat_dto.hpp"
#include "resource_dto.hpp"
#include "alert_rule_dto.hpp"
#include "alert_event_dto.hpp"
#include "query_dto.hpp"
#include "../../domain/entities/server_node.hpp"
#include "../../domain/entities/metric_snapshot.hpp"
#include "../../domain/entities/alert_rule.hpp"
#include "../../domain/entities/alert_event.hpp"
#include <chrono>

namespace monitoring::application {

/**
 * DTOConverter - DTO与领域对象之间的转换器
 * 负责在应用层和领域层之间转换数据
 *
 * 设计原则：
 * - 转换器是无状态的工具类
 * - 只做数据格式转换，不包含业务逻辑
 * - 所有方法都是静态方法
 */
class DTOConverter {
public:
    // ============ Heartbeat DTO -> Domain ============

    /**
     * 从心跳DTO转换为ServerNode领域对象
     */
    static domain::ServerNode toServerNode(const HeartbeatRequestDTO& dto) {
        // 生成节点ID
        domain::ServerNode::HardwareInfo hwInfo = toHardwareInfo(dto.data);
        std::string nodeId = domain::ServerNode::generateNodeId(dto.data.host_ip, hwInfo);

        return domain::ServerNode(
            nodeId,
            dto.data.hostname,
            dto.data.host_ip,
            hwInfo
        );
    }

    /**
     * 从SystemInfoDTO转换为HardwareInfo
     */
    static domain::ServerNode::HardwareInfo toHardwareInfo(const SystemInfoDTO& dto) {
        domain::ServerNode::HardwareInfo hwInfo;
        hwInfo.boxId = dto.box_id;
        hwInfo.slotId = dto.slot_id;
        hwInfo.cpuId = dto.cpu_id;
        hwInfo.srioId = dto.srio_id;
        hwInfo.servicePort = dto.service_port;
        hwInfo.boxType = dto.box_type;
        hwInfo.boardType = dto.board_type;
        hwInfo.cpuType = dto.cpu_type;
        hwInfo.osType = dto.os_type;
        hwInfo.resourceType = dto.resource_type;
        hwInfo.cpuArch = dto.cpu_arch;

        // 转换GPU列表
        for (const auto& gpuDto : dto.gpu) {
            domain::ServerNode::HardwareInfo::GpuDevice gpu;
            gpu.index = gpuDto.index;
            gpu.name = gpuDto.name;
            hwInfo.gpus.push_back(gpu);
        }

        return hwInfo;
    }

    // ============ Resource DTO -> Domain ============

    /**
     * 从资源上报DTO转换为MetricSnapshot领域对象
     *
     * @param dto 资源上报DTO
     * @param nodeId 节点ID（需要从外部传入，因为DTO只包含IP）
     * @return MetricSnapshot领域对象
     */
    static domain::MetricSnapshot toMetricSnapshot(
        const ResourceReportRequestDTO& dto,
        const std::string& nodeId
    ) {
        uint64_t timestamp = getCurrentTimestamp();

        // 转换各个子结构
        domain::CpuMetrics cpu = toCpuMetrics(dto.data.resource.cpu);
        domain::MemoryMetrics memory = toMemoryMetrics(dto.data.resource.memory);
        std::vector<domain::DiskMetrics> disks = toDiskMetrics(dto.data.resource.disk);
        std::vector<domain::NetworkMetrics> networks = toNetworkMetrics(dto.data.resource.network);
        std::vector<domain::GpuMetrics> gpus = toGpuMetrics(dto.data.resource.gpu);
        std::vector<domain::ComponentMetrics> components = toComponentMetrics(dto.data.component);

        return domain::MetricSnapshot(
            nodeId,
            timestamp,
            cpu,
            memory,
            disks,
            networks,
            gpus,
            dto.data.resource.gpu_allocated,
            dto.data.resource.gpu_num,
            components
        );
    }

    /**
     * CPU指标转换
     */
    static domain::CpuMetrics toCpuMetrics(const CpuResourceDTO& dto) {
        domain::CpuMetrics metrics;
        metrics.usagePercent = dto.usage_percent;
        metrics.loadAvg1m = dto.load_avg_1m;
        metrics.loadAvg5m = dto.load_avg_5m;
        metrics.loadAvg15m = dto.load_avg_15m;
        metrics.coreCount = dto.core_count;
        metrics.coreAllocated = dto.core_allocated;
        metrics.temperature = dto.temperature;
        metrics.voltage = dto.voltage;
        metrics.current = dto.current;
        metrics.power = dto.power;
        return metrics;
    }

    /**
     * 内存指标转换
     */
    static domain::MemoryMetrics toMemoryMetrics(const MemoryResourceDTO& dto) {
        domain::MemoryMetrics metrics;
        metrics.totalBytes = dto.total;
        metrics.usedBytes = dto.used;
        metrics.freeBytes = dto.free;
        metrics.usagePercent = dto.usage_percent;
        return metrics;
    }

    /**
     * 磁盘指标转换
     */
    static std::vector<domain::DiskMetrics> toDiskMetrics(
        const std::vector<DiskPartitionDTO>& dtos
    ) {
        std::vector<domain::DiskMetrics> result;
        for (const auto& dto : dtos) {
            domain::DiskMetrics metrics;
            metrics.device = dto.device;
            metrics.mountPoint = dto.mount_point;
            metrics.totalBytes = dto.total;
            metrics.usedBytes = dto.used;
            metrics.freeBytes = dto.free;
            metrics.usagePercent = dto.usage_percent;
            result.push_back(metrics);
        }
        return result;
    }

    /**
     * 网络指标转换
     */
    static std::vector<domain::NetworkMetrics> toNetworkMetrics(
        const std::vector<NetworkInterfaceDTO>& dtos
    ) {
        std::vector<domain::NetworkMetrics> result;
        for (const auto& dto : dtos) {
            domain::NetworkMetrics metrics;
            metrics.interfaceName = dto.interface;
            metrics.rxBytes = dto.rx_bytes;
            metrics.txBytes = dto.tx_bytes;
            metrics.rxPackets = dto.rx_packets;
            metrics.txPackets = dto.tx_packets;
            metrics.rxErrors = dto.rx_errors;
            metrics.txErrors = dto.tx_errors;
            metrics.rxRate = dto.rx_rate;
            metrics.txRate = dto.tx_rate;
            result.push_back(metrics);
        }
        return result;
    }

    /**
     * GPU指标转换
     */
    static std::vector<domain::GpuMetrics> toGpuMetrics(
        const std::vector<GpuResourceDTO>& dtos
    ) {
        std::vector<domain::GpuMetrics> result;
        for (const auto& dto : dtos) {
            domain::GpuMetrics metrics;
            metrics.index = dto.index;
            metrics.name = dto.name;
            metrics.computeUsage = dto.compute_usage;
            metrics.memUsage = dto.mem_usage;
            metrics.memUsed = dto.mem_used;
            metrics.memTotal = dto.mem_total;
            metrics.temperature = dto.temperature;
            metrics.power = dto.power;
            result.push_back(metrics);
        }
        return result;
    }

    /**
     * 组件指标转换
     */
    static std::vector<domain::ComponentMetrics> toComponentMetrics(
        const std::vector<ComponentResourceDTO>& dtos
    ) {
        std::vector<domain::ComponentMetrics> result;
        for (const auto& dto : dtos) {
            domain::ComponentMetrics metrics;
            metrics.instanceId = dto.instance_id;
            metrics.uuid = dto.uuid;
            metrics.index = dto.index;

            metrics.config.name = dto.config.name;
            metrics.config.id = dto.config.id;

            metrics.state = stringToComponentState(dto.state);

            metrics.resource.cpuLoad = dto.resource.cpu_load;
            metrics.resource.memUsed = dto.resource.mem_used;
            metrics.resource.memLimit = dto.resource.mem_limit;
            metrics.resource.networkTx = dto.resource.network_tx;
            metrics.resource.networkRx = dto.resource.network_rx;

            result.push_back(metrics);
        }
        return result;
    }

    // ============ Domain -> DTO 转换方法 ============

    /**
     * 从ServerNode领域对象转换为NodeHealthDTO
     */
    static NodeHealthDTO toNodeHealthDTO(const domain::ServerNode& node) {
        NodeHealthDTO dto;
        dto.nodeId = node.getNodeId();
        dto.hostname = node.getHostname();
        dto.ipAddress = node.getIpAddress();
        dto.status = domain::statusToString(node.getStatus());
        dto.lastSeenAt = node.getLastSeenAt();
        dto.isOnline = node.isOnline();
        return dto;
    }

    /**
     * 从MetricSnapshot领域对象转换为MetricDataDTO
     */
    static MetricDataDTO toMetricDataDTO(const domain::MetricSnapshot& snapshot) {
        MetricDataDTO dto;
        dto.timestamp = snapshot.getTimestamp();
        dto.cpuUsage = snapshot.getCpu().usagePercent;
        dto.memoryUsage = snapshot.getMemory().usagePercent;
        
        // 计算磁盘使用率（取平均值）
        if (!snapshot.getDisks().empty()) {
            double totalDiskUsage = 0.0;
            for (const auto& disk : snapshot.getDisks()) {
                totalDiskUsage += disk.usagePercent;
            }
            dto.diskUsage = totalDiskUsage / snapshot.getDisks().size();
        } else {
            dto.diskUsage = 0.0;
        }
        
        // 计算网络速率（取总和）
        dto.networkRxRate = 0.0;
        dto.networkTxRate = 0.0;
        for (const auto& network : snapshot.getNetworks()) {
            dto.networkRxRate += network.rxRate;
            dto.networkTxRate += network.txRate;
        }
        
        return dto;
    }

    /**
     * 从AlertEvent领域对象转换为AlertEventDTO
     */
    static AlertEventDTO toAlertEventDTO(const domain::AlertEvent& event) {
        AlertEventDTO dto;
        dto.eventId = event.getEventId();
        dto.ruleId = event.getRuleId();
        dto.nodeId = event.getNodeId();
        dto.status = domain::alertStatusToString(event.getStatus());
        dto.severity = domain::severityToString(event.getSeverity());
        dto.startAt = event.getStartAt();
        dto.endAt = event.getEndAt();
        dto.triggeredValue = event.getTriggeredValue();
        dto.details = event.getDetails();
        dto.acknowledgedBy = event.getAcknowledgedBy();
        dto.acknowledgedAt = event.getAcknowledgedAt();
        return dto;
    }

    /**
     * 从AlertRule领域对象转换为AlertRuleDTO
     */
    static AlertRuleDTO toAlertRuleDTO(const domain::AlertRule& rule) {
        AlertRuleDTO dto;
        dto.ruleId = rule.getRuleId();
        dto.ruleName = rule.getRuleName();
        dto.metricName = rule.getMetricName();
        dto.threshold = rule.getThreshold();
        dto.operator_ = domain::operatorToString(rule.getOperator());
        dto.durationSeconds = rule.getDuration();
        dto.severity = domain::severityToString(rule.getSeverity());
        dto.isEnabled = rule.isEnabled();
        dto.description = rule.getDescription();
        dto.createdAt = rule.getCreatedAt();
        dto.updatedAt = rule.getUpdatedAt();
        return dto;
    }

private:
    /**
     * 获取当前Unix时间戳（秒）
     */
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()
        ).count();
    }

    /**
     * 字符串转ComponentState枚举
     */
    static domain::ComponentMetrics::State stringToComponentState(const std::string& str) {
        if (str == "PENDING") return domain::ComponentMetrics::State::PENDING;
        if (str == "RUNNING") return domain::ComponentMetrics::State::RUNNING;
        if (str == "FAILED")  return domain::ComponentMetrics::State::FAILED;
        if (str == "STOPPED") return domain::ComponentMetrics::State::STOPPED;
        if (str == "SLEEPING") return domain::ComponentMetrics::State::SLEEPING;
        return domain::ComponentMetrics::State::PENDING;
    }
};

} // namespace monitoring::application

#endif // APPLICATION_DTO_DTO_CONVERTER_HPP
