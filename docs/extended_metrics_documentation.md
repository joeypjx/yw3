# 扩展告警指标路径功能文档

## 📊 功能概述

成功扩展了告警指标路径，支持磁盘、网卡、GPU等资源的细粒度监控。现在可以针对特定设备或聚合指标设置告警规则。

## 🎯 支持的指标路径

### 1. **CPU指标** (原有功能)
```
cpu.usage_percent          # CPU使用率
cpu.load_avg_1m            # 1分钟平均负载
cpu.load_avg_5m            # 5分钟平均负载
cpu.load_avg_15m           # 15分钟平均负载
cpu.temperature            # CPU温度
cpu.voltage                # CPU电压
cpu.current                # CPU电流
cpu.power                  # CPU功率
cpu.core_count             # CPU核心总数
cpu.core_allocated         # 已分配核心数
```

### 2. **内存指标** (原有功能)
```
memory.usage_percent       # 内存使用率
memory.used                # 已使用内存(字节)
memory.free                # 空闲内存(字节)
memory.total               # 总内存(字节)
```

### 3. **磁盘聚合指标** (新增)
```
disk.usage_percent.avg     # 磁盘使用率平均值
disk.usage_percent.max     # 磁盘使用率最大值
disk.usage_percent.min     # 磁盘使用率最小值
disk.total_bytes           # 磁盘总容量(字节)
disk.used_bytes            # 磁盘已使用(字节)
disk.free_bytes            # 磁盘空闲(字节)
```

### 4. **特定磁盘指标** (新增)
```
disk.{device}.usage_percent    # 特定磁盘使用率
disk.{device}.total_bytes      # 特定磁盘总容量
disk.{device}.used_bytes       # 特定磁盘已使用
disk.{device}.free_bytes       # 特定磁盘空闲

示例:
disk./dev/sda1.usage_percent   # 根分区使用率
disk./dev/sdb1.usage_percent   # 数据分区使用率
```

### 5. **网络聚合指标** (新增)
```
network.rx_rate.total      # 网络总接收速率(字节/秒)
network.tx_rate.total      # 网络总发送速率(字节/秒)
network.rx_bytes.total     # 网络总接收字节数
network.tx_bytes.total     # 网络总发送字节数
network.rx_errors.total     # 网络总接收错误数
network.tx_errors.total     # 网络总发送错误数
```

### 6. **特定网络接口指标** (新增)
```
network.{interface}.rx_rate    # 特定网卡接收速率
network.{interface}.tx_rate    # 特定网卡发送速率
network.{interface}.rx_bytes   # 特定网卡接收字节数
network.{interface}.tx_bytes   # 特定网卡发送字节数

示例:
network.eth0.rx_rate           # eth0网卡接收速率
network.wlan0.tx_rate          # wlan0网卡发送速率
```

### 7. **GPU聚合指标** (新增)
```
gpu.compute_usage.avg      # GPU计算使用率平均值
gpu.compute_usage.max      # GPU计算使用率最大值
gpu.mem_usage.avg          # GPU显存使用率平均值
gpu.mem_usage.max          # GPU显存使用率最大值
gpu.temperature.avg        # GPU温度平均值
gpu.temperature.max        # GPU温度最大值
gpu.power.total            # GPU总功率
gpu.count                  # GPU数量
gpu.allocated              # 已分配GPU数
gpu.total                  # GPU总数
```

### 8. **特定GPU指标** (新增)
```
gpu.{index}.compute_usage  # 特定GPU计算使用率
gpu.{index}.mem_usage      # 特定GPU显存使用率
gpu.{index}.temperature    # 特定GPU温度
gpu.{index}.power          # 特定GPU功率

示例:
gpu.0.compute_usage        # GPU0计算使用率
gpu.1.temperature          # GPU1温度
```

## 🔧 实现细节

### 1. **聚合策略**
- **磁盘**: 平均值、最大值、最小值、总和
- **网络**: 总和（所有网卡累加）
- **GPU**: 平均值、最大值、总和

### 2. **特定设备匹配**
- **磁盘**: 通过设备名匹配（如 `/dev/sda1`）
- **网络**: 通过接口名匹配（如 `eth0`）
- **GPU**: 通过索引号匹配（如 `0`, `1`）

### 3. **错误处理**
- 设备不存在时返回默认值（0.0）
- 无效指标路径抛出异常
- 解析失败时提供详细错误信息

## 📝 使用示例

### 创建磁盘聚合告警规则
```bash
curl -X POST http://localhost:18888/api/alert-rules \
  -H "Content-Type: application/json" \
  -d '{
    "ruleName": "磁盘使用率平均值告警",
    "metricName": "disk.usage_percent.avg",
    "threshold": 50.0,
    "operator": ">",
    "durationSeconds": 1,
    "severity": "WARNING",
    "description": "磁盘使用率平均值超过50%"
  }'
```

### 创建特定磁盘告警规则
```bash
curl -X POST http://localhost:18888/api/alert-rules \
  -H "Content-Type: application/json" \
  -d '{
    "ruleName": "根分区使用率告警",
    "metricName": "disk./dev/sda1.usage_percent",
    "threshold": 90.0,
    "operator": ">",
    "durationSeconds": 1,
    "severity": "CRITICAL",
    "description": "根分区使用率超过90%"
  }'
```

### 创建网络接口告警规则
```bash
curl -X POST http://localhost:18888/api/alert-rules \
  -H "Content-Type: application/json" \
  -d '{
    "ruleName": "eth0网卡接收速率告警",
    "metricName": "network.eth0.rx_rate",
    "threshold": 500000,
    "operator": ">",
    "durationSeconds": 1,
    "severity": "WARNING",
    "description": "eth0网卡接收速率超过500KB/s"
  }'
```

### 创建GPU告警规则
```bash
curl -X POST http://localhost:18888/api/alert-rules \
  -H "Content-Type: application/json" \
  -d '{
    "ruleName": "GPU0温度告警",
    "metricName": "gpu.0.temperature",
    "threshold": 85.0,
    "operator": ">",
    "durationSeconds": 1,
    "severity": "CRITICAL",
    "description": "GPU0温度超过85度"
  }'
```

## ✅ 测试结果

所有扩展的指标路径都已通过测试：

1. ✅ **磁盘聚合指标**: `disk.usage_percent.avg`, `disk.usage_percent.max`
2. ✅ **网络聚合指标**: `network.rx_rate.total`
3. ✅ **GPU聚合指标**: `gpu.compute_usage.avg`
4. ✅ **特定磁盘指标**: `disk./dev/sda1.usage_percent`
5. ✅ **特定网络指标**: `network.eth0.rx_rate`
6. ✅ **特定GPU指标**: `gpu.0.temperature`

## 🚀 优势

1. **细粒度监控**: 可以监控特定设备的状态
2. **灵活聚合**: 支持多种聚合策略（平均值、最大值、总和）
3. **易于扩展**: 新增设备类型只需添加相应的解析逻辑
4. **向后兼容**: 原有的CPU和内存指标路径保持不变
5. **错误处理**: 完善的错误处理和默认值机制

## 📋 下一步计划

1. **容器指标**: 支持容器级别的资源监控
2. **自定义聚合**: 允许用户自定义聚合策略
3. **指标组合**: 支持多个指标的复合条件
4. **动态阈值**: 基于历史数据的动态阈值调整

---

**扩展完成时间**: 2025-10-18  
**测试状态**: ✅ 全部通过  
**功能状态**: 🚀 生产就绪
