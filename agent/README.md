# Monitoring Agent

简单的监控Agent，定期向Server发送心跳和资源数据。

## 功能

- 每60秒发送一次心跳（/heartbeat）
- 每10秒发送一次资源数据（/resource）
- 生成模拟的系统资源数据
- 支持优雅退出（Ctrl+C）

## 依赖

- **C++17** 编译器
- **libcurl** - HTTP客户端库
- **CMake 3.15+**

### 安装依赖

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libcurl4-openssl-dev
```

**CentOS/RHEL:**
```bash
sudo yum install gcc-c++ cmake libcurl-devel
```

**macOS:**
```bash
brew install cmake curl
```

## 编译

```bash
# 在项目根目录
mkdir build && cd build
cmake ..
make

# Agent可执行文件位于: build/agent/monitoring_agent
```

## 运行

### 基本运行（连接到localhost:18888）

```bash
./build/agent/monitoring_agent
```

### 指定服务器地址

```bash
./build/agent/monitoring_agent <server_host> <server_port>

# 例如：
./build/agent/monitoring_agent 192.168.1.100 18888
```

## 配置

配置在 `agent/config/config.hpp` 中，可以修改：

```cpp
// 服务器配置
std::string serverHost = "localhost";
uint16_t serverPort = 18888;

// 上报间隔（秒）
int32_t heartbeatInterval = 60;      // 心跳间隔
int32_t resourceInterval = 10;       // 资源上报间隔

// 节点信息
int32_t boxId = 1;
int32_t slotId = 1;
int32_t cpuId = 1;
std::string hostIp = "192.168.10.29";
std::string hostname = "agent-node-01";
// ...
```

## 运行示例

```bash
$ ./monitoring_agent
==================================================
  Monitoring Agent Starting...
==================================================
Configuration:
  Server: localhost:18888
  Node IP: 192.168.10.29
  Heartbeat Interval: 60s
  Resource Interval: 10s

Sending initial heartbeat...
Successfully sent data to http://localhost:18888/heartbeat
Response: {"code":0,"message":"success"}

Agent is running. Press Ctrl+C to stop.

[1] Sending resource data...
  ✓ Resource data sent successfully
[2] Sending resource data...
  ✓ Resource data sent successfully
[1] Sending heartbeat...
  ✓ Heartbeat sent successfully
^C
Received signal 2, shutting down...

==================================================
  Agent stopped gracefully.
  Total heartbeats sent: 1
  Total resource reports sent: 2
==================================================
```

## 数据格式

### 心跳数据（/heartbeat）

```json
{
  "api_version": 1,
  "data": {
    "box_id": 1,
    "slot_id": 1,
    "cpu_id": 1,
    "srio_id": 0,
    "host_ip": "192.168.10.29",
    "hostname": "agent-node-01",
    "service_port": 23980,
    "box_type": "计算I型",
    "board_type": "GPU",
    "cpu_type": "Intel Core i7",
    "os_type": "Linux 5.15",
    "resource_type": "GPU I",
    "cpu_arch": "x86_64",
    "gpu": [
      {"index": 0, "name": "NVIDIA GeForce RTX 3080"},
      {"index": 1, "name": "NVIDIA GeForce RTX 3080"}
    ]
  }
}
```

### 资源数据（/resource）

包含CPU、内存、磁盘、网络、GPU的使用情况，以及容器组件信息。
详见 `resource.yaml` 接口规范。

## 生成的模拟数据

- **CPU使用率**: 0-100% 随机
- **内存使用率**: 30-90% 随机
- **温度**: 40-80°C 随机
- **GPU使用率**: 0-100% 随机
- 其他指标基于随机数据计算

## 架构

```
agent_main.cpp          # 主程序，定时调度
├── config/             # 配置管理
│   └── config.hpp
├── collector/          # 数据采集（生成模拟数据）
│   ├── collector.hpp
│   └── collector.cpp
└── reporter/           # HTTP上报
    ├── reporter.hpp
    └── reporter.cpp
```

## 故障处理

### 连接失败
```
✗ Failed to send heartbeat: curl_easy_perform() failed: Couldn't connect to server
```
**解决**: 确保Server正在运行，检查防火墙设置

### 404错误
```
✗ Failed to send resource data: HTTP error code: 404
```
**解决**: 先发送心跳注册节点，再发送资源数据

## 开发说明

- **无依赖JSON库**: 使用字符串拼接生成JSON（简单但有效）
- **使用libcurl**: 标准HTTP客户端，跨平台支持好
- **优雅退出**: 捕获SIGINT/SIGTERM信号
- **定时逻辑**: 使用std::chrono精确计时

## 下一步改进

- [ ] 从配置文件读取配置（而非硬编码）
- [ ] 添加真实的系统数据采集（/proc、PDH API）
- [ ] 支持HTTPS和认证
- [ ] 添加重试机制和断线重连
- [ ] 日志输出到文件
