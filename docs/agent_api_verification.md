# Agent接口调用验证

## ✅ 接口端点匹配

Agent会调用以下两个接口，完全符合API规范：

### 1. 心跳接口 `/heartbeat`

**调用位置**:
- `agent_main.cpp:69` - 启动时立即发送（注册节点）
- `agent_main.cpp:101` - 每60秒定期发送

**URL构建**:
```cpp
// config.hpp:44
return "http://" + serverHost + ":" + std::to_string(serverPort) + "/heartbeat";
// 结果: http://localhost:18888/heartbeat
```

**对应API规范**: `node.yaml` 定义的 `POST /heartbeat`

---

### 2. 资源上报接口 `/resource`

**调用位置**:
- `agent_main.cpp:121` - 每10秒定期发送

**URL构建**:
```cpp
// config.hpp:51
return "http://" + serverHost + ":" + std::to_string(serverPort) + "/resource";
// 结果: http://localhost:18888/resource
```

**对应API规范**: `resource.yaml` 定义的 `POST /resource`

---

## ✅ JSON数据格式匹配

### 心跳数据格式验证

**Agent生成的JSON** (`collector.cpp:30-57`):
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

**node.yaml 要求的格式**:
```yaml
HeartbeatRequest:
  properties:
    api_version: integer
    data:
      box_id: integer
      slot_id: integer
      cpu_id: integer
      srio_id: integer
      host_ip: string (ipv4)
      hostname: string
      service_port: integer (uint16)
      box_type: string
      board_type: string
      cpu_type: string
      os_type: string
      resource_type: string
      cpu_arch: string (enum)
      gpu: array of GpuDevice
```

✅ **完全匹配！**

---

### 资源数据格式验证

**Agent生成的JSON** (`collector.cpp:73-190`):
```json
{
  "api_version": 1,
  "data": {
    "host_ip": "192.168.10.29",
    "resource": {
      "cpu": {
        "usage_percent": 23.5,
        "load_avg_1m": 0.75,
        "load_avg_5m": 0.61,
        "load_avg_15m": 0.52,
        "core_count": 16,
        "core_allocated": 8,
        "temperature": 58.2,
        "voltage": 1.1,
        "current": 2.3,
        "power": 25.6
      },
      "memory": {
        "total": 17179869184,
        "used": 8589934592,
        "free": 8589934592,
        "usage_percent": 50.0
      },
      "network": [ ... ],
      "disk": [ ... ],
      "gpu": [ ... ],
      "gpu_allocated": 1,
      "gpu_num": 2
    },
    "component": [ ... ]
  }
}
```

**resource.yaml 要求的格式**:
```yaml
ResourceReportRequest:
  properties:
    api_version: integer
    data:
      host_ip: string (ipv4)
      resource:
        cpu: CpuResource
        memory: MemoryResource
        network: array of NetworkInterface
        disk: array of DiskPartition
        gpu: array of GpuResource
        gpu_allocated: integer
        gpu_num: integer
      component: array of ComponentResource (可选)
```

✅ **完全匹配！**

---

## 调用流程

```
Agent启动
    ↓
立即发送 POST /heartbeat (注册节点)
    ↓
进入主循环
    ↓
    ├─ 每10秒: POST /resource (上报资源数据)
    └─ 每60秒: POST /heartbeat (保持心跳)
```

---

## HTTP请求细节

**请求方法**: POST

**Content-Type**: `application/json`
- 设置位置: `reporter.cpp:35-37`

**超时设置**: 10秒
- 设置位置: `reporter.cpp:42`

**响应处理**:
- 检查HTTP状态码是否为200
- 打印服务器响应内容

---

## 总结

✅ Agent **完全符合** API规范：
1. ✅ 调用正确的端点 (`/heartbeat` 和 `/resource`)
2. ✅ 使用正确的HTTP方法 (POST)
3. ✅ 发送正确的Content-Type (application/json)
4. ✅ JSON结构与 `node.yaml` 和 `resource.yaml` 完全一致
5. ✅ 所有必需字段都已包含
6. ✅ 数据类型正确（整数、字符串、数组等）

Agent准备就绪，可以与Server端进行通信！
