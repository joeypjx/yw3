# 告警事件API文档

## 概述

告警事件API提供了对监控系统中告警事件的完整管理功能，包括查询、状态管理等操作。

## 基础URL

```
http://localhost:18888
```

## API端点

### 1. 获取所有告警事件

**请求**
```http
GET /api/alert-events
```

**响应**
```json
{
  "code": 0,
  "message": "Success",
  "total": 2,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.10.29_1_1_1",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1760725000,
      "endAt": 0,
      "triggeredValue": 85.5,
      "details": "CPU usage was 85.5%, which is above the 80% threshold for 60 seconds.",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

### 2. 获取告警事件详情

**请求**
```http
GET /api/alert-events/{eventId}
```

**参数**
- `eventId` (int64): 告警事件ID

**响应**
```json
{
  "code": 0,
  "message": "Success",
  "event": {
    "eventId": 1,
    "ruleId": 1,
    "nodeId": "192.168.10.29_1_1_1",
    "status": "FIRING",
    "severity": "WARNING",
    "startAt": 1760725000,
    "endAt": 0,
    "triggeredValue": 85.5,
    "details": "CPU usage was 85.5%, which is above the 80% threshold for 60 seconds.",
    "acknowledgedBy": "",
    "acknowledgedAt": 0
  }
}
```

### 3. 根据节点获取告警事件

**请求**
```http
GET /api/alert-events/node/{nodeId}
```

**参数**
- `nodeId` (string): 节点ID

**响应**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.10.29_1_1_1",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1760725000,
      "endAt": 0,
      "triggeredValue": 85.5,
      "details": "CPU usage was 85.5%, which is above the 80% threshold for 60 seconds.",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

### 4. 根据规则获取告警事件

**请求**
```http
GET /api/alert-events/rule/{ruleId}
```

**参数**
- `ruleId` (int32): 规则ID

**响应**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.10.29_1_1_1",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1760725000,
      "endAt": 0,
      "triggeredValue": 85.5,
      "details": "CPU usage was 85.5%, which is above the 80% threshold for 60 seconds.",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

### 5. 获取活跃的告警事件

**请求**
```http
GET /api/alert-events/active
```

**响应**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.10.29_1_1_1",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1760725000,
      "endAt": 0,
      "triggeredValue": 85.5,
      "details": "CPU usage was 85.5%, which is above the 80% threshold for 60 seconds.",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

### 6. 根据状态获取告警事件

**请求**
```http
GET /api/alert-events/status/{status}
```

**参数**
- `status` (string): 告警状态，可选值：
  - `FIRING`: 正在触发
  - `ACKNOWLEDGED`: 已被认知
  - `RESOLVED`: 已解决

**响应**
```json
{
  "code": 0,
  "message": "Success",
  "total": 1,
  "events": [
    {
      "eventId": 1,
      "ruleId": 1,
      "nodeId": "192.168.10.29_1_1_1",
      "status": "FIRING",
      "severity": "WARNING",
      "startAt": 1760725000,
      "endAt": 0,
      "triggeredValue": 85.5,
      "details": "CPU usage was 85.5%, which is above the 80% threshold for 60 seconds.",
      "acknowledgedBy": "",
      "acknowledgedAt": 0
    }
  ]
}
```

### 7. 认知告警事件

**请求**
```http
POST /api/alert-events/{eventId}/acknowledge
Content-Type: application/json

{
  "operatorId": "admin"
}
```

**参数**
- `eventId` (int64): 告警事件ID
- `operatorId` (string): 操作员ID

**响应**
```json
{
  "code": 0,
  "message": "Alert event acknowledged successfully"
}
```

### 8. 解决告警事件

**请求**
```http
POST /api/alert-events/{eventId}/resolve
```

**参数**
- `eventId` (int64): 告警事件ID

**响应**
```json
{
  "code": 0,
  "message": "Alert event resolved successfully"
}
```

## 数据模型

### AlertEventDTO

| 字段 | 类型 | 描述 |
|------|------|------|
| eventId | int64 | 事件ID（主键） |
| ruleId | int32 | 关联的规则ID |
| nodeId | string | 关联的节点ID |
| status | string | 告警状态：FIRING, ACKNOWLEDGED, RESOLVED |
| severity | string | 严重等级：WARNING, CRITICAL |
| startAt | uint64 | 告警开始时间（Unix时间戳） |
| endAt | uint64 | 告警结束时间（0表示未结束） |
| triggeredValue | double | 触发时的指标值 |
| details | string | 详细描述 |
| acknowledgedBy | string | 认知操作员ID |
| acknowledgedAt | uint64 | 认知时间（0表示未认知） |

## 状态说明

### 告警状态

- **FIRING**: 告警正在触发，需要关注
- **ACKNOWLEDGED**: 告警已被操作员认知，但尚未解决
- **RESOLVED**: 告警已解决，问题已修复

### 状态转换

```
FIRING → ACKNOWLEDGED → RESOLVED
   ↓           ↓
   └───────────┘
```

- 只有 `FIRING` 状态的告警可以被认知
- `FIRING` 或 `ACKNOWLEDGED` 状态的告警可以被解决
- 解决操作是幂等的，重复解决不会报错

## 错误码

| 错误码 | 描述 |
|--------|------|
| 0 | 成功 |
| 400 | 请求参数错误 |
| 404 | 告警事件不存在 |
| 500 | 服务器内部错误 |

## 使用示例

### 获取所有活跃告警

```bash
curl -X GET http://localhost:18888/api/alert-events/active
```

### 认知告警事件

```bash
curl -X POST http://localhost:18888/api/alert-events/1/acknowledge \
  -H "Content-Type: application/json" \
  -d '{"operatorId": "admin"}'
```

### 解决告警事件

```bash
curl -X POST http://localhost:18888/api/alert-events/1/resolve
```

### 根据节点查询告警

```bash
curl -X GET http://localhost:18888/api/alert-events/node/192.168.10.29_1_1_1
```
