# 监控系统 API 文档

## 概述

本文档描述了监控系统的所有对外API接口，包括请求格式、响应格式和示例。

**基础信息：**
- 服务地址：`http://localhost:18888`
- 内容类型：`application/json`
- 字符编码：`UTF-8`

---

## 1. 心跳接口

### POST /heartbeat

Agent向Server发送心跳信息，用于节点注册和状态管理。

**请求体：**
```json
{
  "api_version": 1,
  "data": {
    "box_id": 1,
    "slot_id": 0,
    "cpu_id": 0,
    "srio_id": 0,
    "host_ip": "192.168.1.100",
    "hostname": "server-01",
    "service_port": 8080,
    "box_type": "standard",
    "board_type": "x86_64",
    "cpu_type": "Intel Xeon",
    "os_type": "Linux",
    "resource_type": "compute",
    "cpu_arch": "x86_64",
    "gpu": [
      {
        "index": 0,
        "name": "NVIDIA Tesla V100"
      },
      {
        "index": 1,
        "name": "NVIDIA Tesla V100"
      }
    ]
  }
}
```

**响应体：**
```json
{
  "code": 0,
  "message": "Success"
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

---

## 2. 资源上报接口

### POST /resource

Agent向Server上报系统资源使用情况。

**请求体：**
```json
{
  "api_version": 1,
  "data": {
    "host_ip": "192.168.1.100",
    "resource": {
      "cpu": {
        "usage_percent": 45.2,
        "load_avg_1m": 1.2,
        "load_avg_5m": 1.5,
        "load_avg_15m": 1.8,
        "core_count": 16,
        "core_allocated": 8,
        "temperature": 55.5,
        "voltage": 1.2,
        "current": 50.0,
        "power": 60.0
      },
      "memory": {
        "total": 8589934592,
        "used": 4294967296,
        "free": 4294967296,
        "usage_percent": 50.0
      },
      "disk": [
        {
          "device": "/dev/sda1",
          "mount_point": "/",
          "total": 1099511627776,
          "used": 549755813888,
          "free": 549755813888,
          "usage_percent": 50.0
        }
      ],
      "network": [
        {
          "interface": "eth0",
          "rx_bytes": 1024000,
          "tx_bytes": 2048000,
          "rx_packets": 10000,
          "tx_packets": 15000,
          "rx_errors": 0,
          "tx_errors": 0,
          "rx_rate": 1024,
          "tx_rate": 2048
        }
      ],
      "gpu": [
        {
          "index": 0,
          "name": "NVIDIA Tesla V100",
          "compute_usage": 75.5,
          "mem_usage": 60.0,
          "mem_used": 8589934592,
          "mem_total": 17179869184,
          "temperature": 65.0,
          "power": 250.0
        }
      ],
      "gpu_allocated": 1,
      "gpu_num": 2
    },
    "component": [
      {
        "instance_id": "comp-001",
        "uuid": "550e8400-e29b-41d4-a716-446655440000",
        "index": 0,
        "config": {
          "name": "service-a",
          "id": "svc-a-001"
        },
        "state": "RUNNING",
        "resource": {
          "cpu_load": 35.5,
          "mem_used": 536870912,
          "mem_limit": 1073741824,
          "network_tx": 102400,
          "network_rx": 204800
        }
      }
    ]
  }
}
```

**响应体：**
```json
{
  "code": 0,
  "message": "Success"
}
```

**状态码：**
- `200` - 成功
- `404` - 节点不存在
- `500` - 服务器内部错误

---

## 3. 健康检查接口

### GET /

服务器健康状态检查。

**请求体：** 无

**响应体：**
```json
{
  "status": "ok",
  "service": "monitoring-server"
}
```

**状态码：**
- `200` - 服务正常

---

## 4. 告警规则管理 API

### 4.1 创建告警规则

#### POST /api/alert-rules

创建新的告警规则（支持多条件）。

**请求体：**
```json
{
  "alert_name": "CPU使用率告警",
  "alert_type": "resource",
  "description": "当CPU使用率超过80%时触发告警",
  "severity": "WARNING",
  "enabled": true,
  "summary": "CPU使用率过高",
  "for": "5m",
  "expression": {
    "metric": "usage_percent",
    "stable": "cpu",
    "conditions": [
      {
        "operator": ">",
        "threshold": 80.0
      }
    ],
    "tags": [
      {
        "environment": "production"
      },
      {
        "team": "ops"
      }
    ]
  }
}
```

**多条件示例：**
```json
{
  "alert_name": "系统综合告警",
  "alert_type": "resource",
  "description": "CPU和内存同时告警",
  "severity": "CRITICAL",
  "enabled": true,
  "summary": "系统资源紧张",
  "for": "1m",
  "expression": {
    "metric": "usage_percent",
    "stable": "cpu",
    "conditions": [
      {
        "operator": ">",
        "threshold": 90.0
      },
      {
        "operator": ">",
        "threshold": 85.0
      }
    ],
    "tags": [
      {
        "environment": "production"
      }
    ]
  }
}
```

**指定设备示例：**
```json
{
  "alert_name": "GPU0温度告警",
  "alert_type": "hardware",
  "description": "GPU0温度过高告警",
  "severity": "WARNING",
  "enabled": true,
  "summary": "GPU温度异常",
  "for": "2m",
  "expression": {
    "metric": "0.temperature",
    "stable": "gpu",
    "conditions": [
      {
        "operator": ">",
        "threshold": 85.0
      }
    ],
    "tags": [
      {
        "environment": "production"
      }
    ]
  }
}
```

**响应体：**
```json
{
  "code": 0,
  "message": "Alert rule created successfully",
  "rule": {
    "id": "1",
    "alert_name": "CPU使用率告警",
    "alert_type": "resource",
    "description": "当CPU使用率超过80%时触发告警",
    "severity": "WARNING",
    "enabled": true,
    "summary": "CPU使用率过高",
    "for": "5m",
    "expression": {
      "metric": "usage_percent",
      "stable": "cpu",
      "conditions": [
        {
          "operator": ">",
          "threshold": 80.0
        }
      ],
      "tags": [
        {
          "environment": "production"
        },
        {
          "team": "ops"
        }
      ]
    },
    "created_at": "2024-01-01T12:00:00Z",
    "updated_at": "2024-01-01T12:00:00Z"
  }
}
```

**状态码：**
- `201` - 创建成功
- `400` - 请求参数错误
- `500` - 服务器内部错误

### 4.2 更新告警规则

#### PUT /api/alert-rules/{id}

更新指定的告警规则。

**路径参数：**
- `id` - 告警规则ID（字符串）

**请求体：**
```json
{
  "alert_name": "CPU使用率告警（更新）",
  "alert_type": "resource",
  "description": "当CPU使用率超过85%时触发告警",
  "severity": "CRITICAL",
  "enabled": true,
  "summary": "CPU使用率严重过高",
  "for": "5m",
  "expression": {
    "metric": "usage_percent",
    "stable": "cpu",
    "conditions": [
      {
        "operator": ">",
        "threshold": 85.0
      }
    ],
    "tags": [
      {
        "environment": "production"
      },
      {
        "team": "ops"
      },
      {
        "updated": "true"
      }
    ]
  }
}
```

**响应体：**
```json
{
  "code": 0,
  "message": "Alert rule updated successfully",
  "rule": {
    "id": "1",
    "alert_name": "CPU使用率告警（更新）",
    "alert_type": "resource",
    "description": "当CPU使用率超过85%时触发告警",
    "severity": "CRITICAL",
    "enabled": true,
    "summary": "CPU使用率严重过高",
    "for": "5m",
    "expression": {
      "metric": "usage_percent",
      "stable": "cpu",
      "conditions": [
        {
          "operator": ">",
          "threshold": 85.0
        }
      ],
      "tags": [
        {
          "environment": "production"
        },
        {
          "team": "ops"
        },
        {
          "updated": "true"
        }
      ]
    },
    "created_at": "2024-01-01T12:00:00Z",
    "updated_at": "2024-01-01T12:05:00Z"
  }
}
```

**状态码：**
- `200` - 更新成功
- `404` - 规则不存在
- `500` - 服务器内部错误

### 4.3 删除告警规则

#### DELETE /api/alert-rules/{id}

删除指定的告警规则。

**路径参数：**
- `id` - 告警规则ID（字符串）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Alert rule deleted successfully"
}
```

**状态码：**
- `200` - 删除成功
- `404` - 规则不存在
- `500` - 服务器内部错误

### 4.4 获取告警规则列表

#### GET /api/alert-rules

获取所有告警规则列表。

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 2,
  "rules": [
    {
      "id": "1",
      "alert_name": "CPU使用率告警",
      "alert_type": "resource",
      "description": "当CPU使用率超过80%时触发告警",
      "severity": "WARNING",
      "enabled": true,
      "summary": "CPU使用率过高",
      "for": "5m",
      "expression": {
        "metric": "usage_percent",
        "stable": "cpu",
        "conditions": [
          {
            "operator": ">",
            "threshold": 80.0
          }
        ],
        "tags": [
          {
            "environment": "production"
          },
          {
            "team": "ops"
          }
        ]
      },
      "created_at": "2024-01-01T12:00:00Z",
      "updated_at": "2024-01-01T12:00:00Z"
    },
    {
      "id": "2",
      "alert_name": "内存使用率告警",
      "alert_type": "resource",
      "description": "当内存使用率超过90%时触发告警",
      "severity": "CRITICAL",
      "enabled": false,
      "summary": "内存使用率严重过高",
      "for": "3m",
      "expression": {
        "metric": "usage_percent",
        "stable": "memory",
        "conditions": [
          {
            "operator": ">",
            "threshold": 90.0
          }
        ],
        "tags": [
          {
            "environment": "production"
          },
          {
            "team": "ops"
          }
        ]
      },
      "created_at": "2024-01-01T12:01:00Z",
      "updated_at": "2024-01-01T12:02:00Z"
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

### 4.5 获取告警规则详情

#### GET /api/alert-rules/{id}

获取指定告警规则的详细信息。

**路径参数：**
- `id` - 告警规则ID（字符串）

**请求体：** 无

**响应体:**
```json
{
  "code": 0,
  "message": "Success",
  "rule": {
    "id": "1",
    "alert_name": "CPU使用率告警",
    "alert_type": "resource",
    "description": "当CPU使用率超过80%时触发告警",
    "severity": "WARNING",
    "enabled": true,
    "summary": "CPU使用率过高",
    "for": "5m",
    "expression": {
      "metric": "usage_percent",
      "stable": "cpu",
      "conditions": [
        {
          "operator": ">",
          "threshold": 80.0
        }
      ],
      "tags": [
        {
          "environment": "production"
        },
        {
          "team": "ops"
        }
      ]
    },
    "created_at": "2024-01-01T12:00:00Z",
    "updated_at": "2024-01-01T12:00:00Z"
  }
}
```

**状态码：**
- `200` - 成功
- `404` - 规则不存在
- `500` - 服务器内部错误

### 4.6 启用告警规则

#### POST /api/alert-rules/{id}/enable

启用指定的告警规则。

**路径参数：**
- `id` - 告警规则ID（字符串）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Alert rule enabled successfully"
}
```

**状态码：**
- `200` - 启用成功
- `404` - 规则不存在
- `500` - 服务器内部错误

### 4.7 禁用告警规则

#### POST /api/alert-rules/{id}/disable

禁用指定的告警规则。

**路径参数：**
- `id` - 告警规则ID（字符串）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Alert rule disabled successfully"
}
```

**状态码：**
- `200` - 禁用成功
- `404` - 规则不存在
- `500` - 服务器内部错误

### 4.8 获取启用的告警规则

#### GET /api/alert-rules/active

获取所有启用的告警规则列表。

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "rules": [
    {
      "id": "1",
      "alert_name": "CPU使用率告警",
      "alert_type": "resource",
      "description": "当CPU使用率超过80%时触发告警",
      "severity": "WARNING",
      "enabled": true,
      "summary": "CPU使用率过高",
      "for": "5m",
      "expression": {
        "metric": "usage_percent",
        "stable": "cpu",
        "conditions": [
          {
            "operator": ">",
            "threshold": 80.0
          }
        ],
        "tags": [
          {
            "environment": "production"
          },
          {
            "team": "ops"
          }
        ]
      },
      "created_at": "2024-01-01T12:00:00Z",
      "updated_at": "2024-01-01T12:00:00Z"
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

---

## 5. 告警事件管理 API

### 5.1 获取所有告警事件

#### GET /api/alert-events

获取所有告警事件列表。

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 2,
  "events": [
    {
      "id": "1",
      "fingerprint": "1,192.168.10.29_1_1_1",
      "status": "firing",
      "created_at": "2024-01-01T12:00:00Z",
      "starts_at": "2024-01-01T12:00:00Z",
      "ends_at": "",
      "updated_at": "2024-01-01T12:00:00Z",
      "annotations": {
        "description": "All conditions met (AND logic) - High CPU Usage",
        "summary": "CPU使用率过高"
      },
      "labels": {
        "alert_type": "resource",
        "alertname": "High CPU Usage",
        "host_ip": "192.168.10.29",
        "metrics": "cpu.usage_percent",
        "severity": "WARNING",
        "value": "85.200000"
      }
    },
    {
      "id": "2",
      "fingerprint": "2,192.168.10.30_1_1_1",
      "status": "acknowledged",
      "created_at": "2024-01-01T12:01:00Z",
      "starts_at": "2024-01-01T12:01:00Z",
      "ends_at": "",
      "updated_at": "2024-01-01T12:02:00Z",
      "annotations": {
        "description": "All conditions met (AND logic) - High Memory Usage",
        "summary": "内存使用率严重过高"
      },
      "labels": {
        "alert_type": "resource",
        "alertname": "High Memory Usage",
        "host_ip": "192.168.10.30",
        "metrics": "memory.usage_percent",
        "severity": "CRITICAL",
        "value": "92.100000"
      }
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

### 5.2 获取告警事件详情

#### GET /api/alert-events/{id}

获取指定告警事件的详细信息。

**路径参数：**
- `id` - 告警事件ID（字符串）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "event": {
    "id": "1",
    "fingerprint": "1,192.168.10.29_1_1_1",
    "status": "firing",
    "created_at": "2024-01-01T12:00:00Z",
    "starts_at": "2024-01-01T12:00:00Z",
    "ends_at": "",
    "updated_at": "2024-01-01T12:00:00Z",
    "annotations": {
      "description": "All conditions met (AND logic) - High CPU Usage",
      "summary": "CPU使用率过高"
    },
    "labels": {
      "alert_type": "resource",
      "alertname": "High CPU Usage",
      "host_ip": "192.168.10.29",
      "metrics": "cpu.usage_percent",
      "severity": "WARNING",
      "value": "85.200000"
    }
  }
}
```

**状态码：**
- `200` - 成功
- `404` - 事件不存在
- `500` - 服务器内部错误

### 5.3 根据节点获取告警事件

#### GET /api/alert-events/node/{nodeId}

获取指定节点的所有告警事件。

**路径参数：**
- `nodeId` - 节点ID（字符串）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.1.100",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1703123456789,
      "endAt": 0,
      "triggeredValue": 85.2,
      "details": "CPU使用率超过阈值: 85.2% > 80.0%",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

### 5.4 根据规则获取告警事件

#### GET /api/alert-events/rule/{ruleId}

获取指定规则的所有告警事件。

**路径参数：**
- `ruleId` - 告警规则ID（整数）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.1.100",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1703123456789,
      "endAt": 0,
      "triggeredValue": 85.2,
      "details": "CPU使用率超过阈值: 85.2% > 80.0%",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

### 5.5 获取活跃的告警事件

#### GET /api/alert-events/active

获取所有状态为活跃的告警事件。

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.1.100",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1703123456789,
      "endAt": 0,
      "triggeredValue": 85.2,
      "details": "CPU使用率超过阈值: 85.2% > 80.0%",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

### 5.6 根据状态获取告警事件

#### GET /api/alert-events/status/{status}

根据状态获取告警事件（状态值：active, acknowledged, resolved）。

**路径参数：**
- `status` - 告警事件状态（字符串，可选值：active, acknowledged, resolved）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.1.100",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1703123456789,
      "endAt": 0,
      "triggeredValue": 85.2,
      "details": "CPU使用率超过阈值: 85.2% > 80.0%",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `400` - 状态参数无效
- `500` - 服务器内部错误

### 5.7 认知告警事件

#### POST /api/alert-events/{id}/acknowledge

认知指定的告警事件。

**路径参数：**
- `id` - 告警事件ID（整数）

**请求体：**
```json
{
  "operatorId": "admin"
}
```

**响应体：**
```json
{
  "code": 0,
  "message": "Alert event acknowledged successfully"
}
```

**状态码：**
- `200` - 认知成功
- `404` - 事件不存在
- `500` - 服务器内部错误

### 5.8 解决告警事件

#### POST /api/alert-events/{id}/resolve

解决指定的告警事件。

**路径参数：**
- `id` - 告警事件ID（整数）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Alert event resolved successfully"
}
```

**状态码：**
- `200` - 解决成功
- `404` - 事件不存在
- `500` - 服务器内部错误

---

## 6. 节点管理 API

### 6.1 获取所有节点

#### GET /api/nodes

获取所有注册的节点列表。

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 2,
  "nodes": [
    {
      "nodeId": "192.168.1.100",
      "hostname": "server-01",
      "ipAddress": "192.168.1.100",
      "status": "online",
      "lastSeenAt": 1703123456789,
      "isOnline": true
    },
    {
      "nodeId": "192.168.1.101",
      "hostname": "server-02",
      "ipAddress": "192.168.1.101",
      "status": "offline",
      "lastSeenAt": 1703123456000,
      "isOnline": false
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

### 6.2 获取节点详情

#### GET /api/nodes/{nodeId}

获取指定节点的详细信息。

**路径参数：**
- `nodeId` - 节点ID（字符串）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "node": {
    "nodeId": "192.168.1.100",
    "hostname": "server-01",
    "ipAddress": "192.168.1.100",
    "status": "online",
    "lastSeenAt": 1703123456789,
    "isOnline": true
  }
}
```

**状态码：**
- `200` - 成功
- `404` - 节点不存在
- `500` - 服务器内部错误

### 6.3 根据状态获取节点

#### GET /api/nodes/status/{status}

根据状态获取节点列表。

**路径参数：**
- `status` - 节点状态（字符串，可选值：HEALTHY, WARNING, CRITICAL, UNKNOWN, online, offline）

**注意：** API根据NodeHealthDTO的status字段过滤，可能的值包括健康状态（HEALTHY/WARNING/CRITICAL/UNKNOWN）或在线状态（online/offline）

**请求体：** 无

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "nodes": [
    {
      "nodeId": "192.168.1.100",
      "hostname": "server-01",
      "ipAddress": "192.168.1.100",
      "status": "online",
      "lastSeenAt": 1703123456789,
      "isOnline": true
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

---

## 7. 指标查询 API

### 7.1 获取节点指标数据

#### GET /api/nodes/{nodeId}/metrics

获取指定节点的指标数据。

**路径参数：**
- `nodeId` - 节点ID（字符串）

**查询参数：**
- `startTime` (可选): 开始时间戳（毫秒）
- `endTime` (可选): 结束时间戳（毫秒）
- `recentSeconds` (可选): 最近N秒的数据，默认300秒

**示例请求：**
```
GET /api/nodes/192.168.1.100/metrics?startTime=1703123456000&endTime=1703123457000
GET /api/nodes/192.168.1.100/metrics?recentSeconds=600
```

**响应体：**
```json
{
  "code": 0,
  "message": "Success",
  "total": 2,
  "metrics": [
    {
      "timestamp": 1703123456789,
      "cpuUsage": 45.2,
      "memoryUsage": 50.0,
      "diskUsage": 30.5,
      "networkRxRate": 1024.5,
      "networkTxRate": 2048.3
    },
    {
      "timestamp": 1703123456790,
      "cpuUsage": 47.8,
      "memoryUsage": 52.1,
      "diskUsage": 31.2,
      "networkRxRate": 1156.7,
      "networkTxRate": 2134.9
    }
  ]
}
```

**状态码：**
- `200` - 成功
- `500` - 服务器内部错误

---

## 8. 错误响应格式

所有API在发生错误时都会返回统一的错误格式：

```json
{
  "code": 500,
  "message": "Internal server error"
}
```

**常见错误码：**
- `400` - 请求参数错误
- `404` - 资源不存在
- `500` - 服务器内部错误

---

## 9. 数据模型说明

### 9.1 告警规则条件

**告警条件结构：**
```json
{
  "operator": ">",                   // 比较操作符: >, <, >=, <=, ==, !=
  "threshold": 80.0                  // 阈值
}
```

**告警表达式结构：**
```json
{
  "metric": "cpu.usage_percent",     // 指标名称
  "stable": "node",                  // 数据域（可选）
  "conditions": [...],               // 条件数组
  "tags": [                          // 全局标签（可选）
    {
      "key": "environment",
      "value": "production"
    }
  ]
}
```

**标签匹配功能：**

告警规则支持强大的标签匹配功能，可以根据节点的各种属性进行精确过滤：

| 标签键 | 类型 | 说明 | 示例值 |
|--------|------|------|--------|
| `node_id` | string | 完整节点ID | `"192.168.10.29_1_1_1"` |
| `host_ip` | string | 主机IP地址 | `"192.168.10.29"` |
| `hostname` | string | 主机名 | `"test-host"` |
| `box_id` | int | 机箱ID | `"1"` |
| `slot_id` | int | 槽位ID | `"1"` |
| `cpu_id` | int | CPU ID | `"1"` |
| `srio_id` | int | SRIO ID | `"1"` |
| `service_port` | int | 服务端口 | `"8080"` |
| `box_type` | string | 机箱类型 | `"standard"` |
| `board_type` | string | 板卡类型 | `"compute"` |
| `cpu_type` | string | CPU类型 | `"x86_64"` |
| `os_type` | string | 操作系统类型 | `"linux"` |
| `resource_type` | string | 资源类型 | `"compute"` |
| `cpu_arch` | string | CPU架构 | `"x86_64"` |
| `gpu_count` | int | GPU数量 | `"2"` |

**标签匹配示例：**

基于IP地址的告警规则：
```json
{
  "expression": {
    "metric": "usage_percent",
    "stable": "cpu",
    "conditions": [{"operator": ">", "threshold": 80.0}],
    "tags": [
      {"host_ip": "192.168.10.29"}
    ]
  }
}
```

基于机箱ID的告警规则：
```json
{
  "expression": {
    "metric": "usage_percent",
    "stable": "memory",
    "conditions": [{"operator": ">", "threshold": 85.0}],
    "tags": [
      {"box_id": "1"}
    ]
  }
}
```

多标签组合匹配：
```json
{
  "expression": {
    "metric": "usage_percent",
    "stable": "cpu",
    "conditions": [{"operator": ">", "threshold": 95.0}],
    "tags": [
      {"box_id": "1"},
      {"os_type": "linux"},
      {"resource_type": "production"}
    ]
  }
}
```

**告警规则结构：**
```json
{
  "id": "1",                         // 规则ID（字符串）
  "alert_name": "CPU使用率告警",      // 告警名称
  "alert_type": "resource",          // 告警类型
  "description": "描述",             // 描述
  "severity": "WARNING",             // 严重程度
  "enabled": true,                   // 是否启用
  "summary": "摘要",                  // 摘要
  "for": "5m",                       // 持续时间
  "expression": {...},               // 表达式
  "created_at": "2024-01-01T12:00:00Z", // 创建时间
  "updated_at": "2024-01-01T12:00:00Z"  // 更新时间
}
```

**支持的指标：**
- `cpu.usage_percent` - CPU使用率
- `memory.usage_percent` - 内存使用率
- `disk.{device}.usage_percent` - 磁盘使用率
- `disk.{device}.read_rate` - 磁盘读取速率
- `disk.{device}.write_rate` - 磁盘写入速率
- `network.{interface}.rx_rate` - 网络接收速率
- `network.{interface}.tx_rate` - 网络发送速率
- `gpu.{device}.temperature` - GPU温度
- `gpu.{device}.compute_usage` - GPU计算使用率
- `gpu.{device}.mem_usage` - GPU内存使用率

**指标格式说明：**
- 在API请求中，使用 `stable` 和 `metric` 分离格式：
  - `stable: "cpu"`, `metric: "usage_percent"` → 完整指标名：`cpu.usage_percent`
  - `stable: "gpu"`, `metric: "0.temperature"` → 完整指标名：`gpu.0.temperature`
- 在API响应中，返回分离的 `stable` 和 `metric` 字段

### 9.2 逻辑操作符

- `AND` - 所有条件都必须满足
- `OR` - 任一条件满足即可

### 9.3 告警严重级别

- `info` - 信息级别
- `warning` - 警告级别
- `critical` - 严重级别

### 9.4 告警事件状态

- `FIRING` - 活跃状态（正在触发）
- `ACKNOWLEDGED` - 已认知
- `RESOLVED` - 已解决

### 9.5 告警事件字段说明

**告警事件结构：**
```json
{
  "id": "1",                         // 事件ID（字符串）
  "fingerprint": "1,192.168.10.29_1_1_1", // 指纹（ruleId,nodeId组合）
  "status": "firing",                // 状态：firing, acknowledged, resolved
  "created_at": "2024-01-01T12:00:00Z", // 创建时间（ISO 8601格式）
  "starts_at": "2024-01-01T12:00:00Z",  // 开始时间（ISO 8601格式）
  "ends_at": "",                     // 结束时间（ISO 8601格式，空表示未结束）
  "updated_at": "2024-01-01T12:00:00Z", // 更新时间（ISO 8601格式）
  "annotations": {                   // 注释
    "description": "详细描述",
    "summary": "摘要"
  },
  "labels": {                        // 标签
    "alert_type": "resource",        // 告警类型
    "alertname": "High CPU Usage",   // 告警名称
    "host_ip": "192.168.10.29",      // 主机IP
    "metrics": "cpu.usage_percent",  // 指标名称
    "severity": "WARNING",           // 严重等级
    "value": "85.200000"             // 触发时的指标值
  }
}
```

---

## 10. 使用示例

### 10.1 创建CPU告警规则

```bash
curl -X POST http://localhost:18888/api/alert-rules \
  -H "Content-Type: application/json" \
  -d '{
    "alert_name": "CPU使用率告警",
    "alert_type": "resource",
    "description": "当CPU使用率超过80%时触发告警",
    "severity": "WARNING",
    "enabled": true,
    "summary": "CPU使用率过高",
    "for": "5m",
    "expression": {
      "metric": "usage_percent",
      "stable": "cpu",
      "conditions": [
        {
          "operator": ">",
          "threshold": 80.0
        }
      ],
      "tags": [
        {
          "environment": "production"
        }
      ]
    }
  }'
```

### 10.2 查询告警事件

```bash
curl -X GET http://localhost:18888/api/alert-events/active
```

### 10.3 获取节点指标

```bash
curl -X GET "http://localhost:18888/api/nodes/192.168.1.100/metrics?recentSeconds=300"
```

---

## 11. 注意事项

1. **时间戳格式**: 所有时间戳均为ISO 8601格式（YYYY-MM-DD HH:MM:SS）
2. **字符编码**: 所有文本字段使用UTF-8编码
3. **数值精度**: 浮点数保留6位小数
4. **设备标识**: 设备名称区分大小写
5. **ID格式**: 规则ID和事件ID均为字符串格式
6. **标签格式**: 标签使用对象格式 `{"key": "value"}` 而不是 `{"key": "key", "value": "value"}`
7. **指标格式**: 使用 `stable` 和 `metric` 分离格式，系统会自动组合为完整指标名
8. **并发限制**: 建议控制并发请求数量，避免服务器过载
9. **错误重试**: 建议实现指数退避的重试机制
10. **数据验证**: 客户端应验证响应数据的完整性

---

## 12. 更新日志

- **v1.0.0** (2024-01-01): 初始版本，支持基础告警规则和事件管理
- **v1.1.0** (2024-01-15): 新增多条件告警规则支持
- **v1.2.0** (2024-01-30): 新增设备指定监控功能
- **v2.2.0** (2024-10-18): 优化API格式和标签结构
  - 更新告警规则API格式，使用 `stable` 和 `metric` 分离设计
  - 优化标签格式，使用对象格式 `{"key": "value"}` 替代 `{"key": "key", "value": "value"}`
  - 更新所有API示例和响应格式
  - 完善指标格式说明和组合规则
  - 增强API文档的准确性和一致性
