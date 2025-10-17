# 数据转换流程详解

## 概述

本文档说明 HTTP JSON 请求如何一步步转换为领域对象，体现了分层架构和依赖倒置原则。

## 完整转换链路

```
HTTP JSON 字符串 → DTO对象 → 领域对象 → 数据库
   (表现层)      (应用层)    (领域层)   (基础设施层)
```

## 各层职责

### 1. 表现层 (Presentation Layer)

**位置**: `server/presentation/controllers/`

**职责**:
- 接收 HTTP 请求，提取 JSON 字符串
- 使用 `JsonUtils` 将 JSON 反序列化为 DTO
- 调用应用层服务
- 将响应 DTO 序列化为 JSON 返回

**关键类**:
- `JsonUtils`: JSON 序列化/反序列化工具（需要 nlohmann/json 库）
- `HttpApiController`: HTTP 端点控制器

**示例**:
```cpp
// 接收JSON字符串
std::string jsonBody = request.getBody();

// JSON → DTO
auto dto = JsonUtils::parseHeartbeatRequest(jsonBody);

// 调用应用层
auto response = ingestionService_->processHeartbeat(dto);

// DTO → JSON
std::string responseJson = JsonUtils::serializeHeartbeatResponse(response);
```

### 2. 应用层 (Application Layer)

**位置**: `server/application/`

**职责**:
- 接收 DTO，使用 `DTOConverter` 转换为领域对象
- 协调领域对象和仓储完成业务用例
- 不包含业务规则，只负责编排

**关键类**:
- `DTOConverter`: DTO ↔ Domain 转换器（静态工具类）
- `HeartbeatRequestDTO`, `ResourceReportRequestDTO`: 数据传输对象
- `IngestionService`, `QueryService`: 应用服务

**示例**:
```cpp
// DTO → Domain
domain::ServerNode node = DTOConverter::toServerNode(dto);

// 调用领域对象的业务方法
node.updateLastSeen();
node.updateStatus(Status::HEALTHY);

// 通过仓储接口持久化
nodeRepository_->save(node);
```

### 3. 领域层 (Domain Layer)

**位置**: `server/domain/`

**职责**:
- 封装核心业务逻辑和规则
- 提供业务方法（如 `updateLastSeen()`, `evaluate()`）
- 定义仓储接口（不实现）

**关键类**:
- `ServerNode`, `MetricSnapshot`: 实体
- `AlertRule`, `AlertEvent`: 实体（含业务逻辑）
- `IServerNodeRepository`, `IMetricRepository`: 仓储接口

**示例**:
```cpp
// 领域对象自己管理业务规则
void ServerNode::updateLastSeen() {
    lastSeenAt_ = getCurrentTimestamp();
}

// 领域对象包含复杂业务逻辑
EvaluationResult AlertRule::evaluate(
    const ServerNode& node,
    const std::vector<MetricSnapshot>& metrics
) const {
    // 检查数据新鲜度、覆盖率、阈值判断等
    // ...
}
```

### 4. 基础设施层 (Infrastructure Layer)

**位置**: `server/infrastructure/repositories/`

**职责**:
- 实现领域层定义的仓储接口
- 将领域对象转换为数据库格式并存储
- 从数据库读取数据并转换为领域对象

**关键类**:
- `PostgresNodeRepository`: 实现 `IServerNodeRepository`
- `InfluxDBMetricRepository`: 实现 `IMetricRepository`

## 具体转换示例

### 示例1: 心跳上报 (/heartbeat)

```cpp
// ========== 表现层 ==========
std::string jsonBody = R"({
  "api_version": 1,
  "data": {
    "box_id": 1,
    "slot_id": 1,
    "cpu_id": 1,
    "host_ip": "192.168.10.29",
    "hostname": "node-01",
    ...
  }
})";

// JSON → DTO
HeartbeatRequestDTO dto = JsonUtils::parseHeartbeatRequest(jsonBody);

// ========== 应用层 ==========
// DTO → Domain
ServerNode node = DTOConverter::toServerNode(dto);
// node.nodeId = "192.168.10.29_1_1_1"
// node.hostname = "node-01"
// node.ipAddress = "192.168.10.29"
// node.hardwareInfo.boxId = 1
// ...

// ========== 领域层 ==========
// 业务逻辑
auto existing = nodeRepository_->findById(node.getNodeId());
if (existing) {
    existing->updateLastSeen();  // 业务方法
    existing->updateStatus(Status::HEALTHY);
}

// ========== 基础设施层 ==========
// 仓储实现将领域对象转为SQL并保存
nodeRepository_->save(node);
// 执行: UPDATE server_nodes SET last_seen_at = NOW() WHERE node_id = '...'
```

### 示例2: 资源上报 (/resource)

```cpp
// ========== 表现层 ==========
std::string jsonBody = R"({
  "api_version": 1,
  "data": {
    "host_ip": "192.168.10.29",
    "resource": {
      "cpu": {"usage_percent": 23.5, ...},
      "memory": {"total": 34359738368, ...},
      ...
    }
  }
})";

ResourceReportRequestDTO dto = JsonUtils::parseResourceRequest(jsonBody);

// ========== 应用层 ==========
// 先查找节点
auto nodes = nodeRepository_->findByIpAddress(dto.data.host_ip);
std::string nodeId = nodes[0].getNodeId();

// DTO → Domain
MetricSnapshot snapshot = DTOConverter::toMetricSnapshot(dto, nodeId);
// snapshot.nodeId = "192.168.10.29_1_1_1"
// snapshot.timestamp = 1704567890
// snapshot.cpu.usagePercent = 23.5
// snapshot.memory.totalBytes = 34359738368
// ...

// ========== 基础设施层 ==========
// 保存到InfluxDB
metricRepository_->save(snapshot);
// 写入时序数据: measurement=cpu, tags={nodeId=...}, fields={usage_percent=23.5}, time=...
```

## 为什么这样设计？

### 1. 关注点分离
- **表现层**: 只关心HTTP和JSON格式
- **应用层**: 只关心用例编排和数据转换
- **领域层**: 只关心业务逻辑和规则
- **基础设施层**: 只关心数据库技术细节

### 2. 依赖倒置
- 领域层定义接口（如 `IServerNodeRepository`）
- 应用层依赖这些接口
- 基础设施层实现这些接口
- **高层不依赖低层，都依赖抽象**

### 3. 可测试性
```cpp
// 可以轻松Mock仓储接口进行单元测试
class MockNodeRepository : public IServerNodeRepository {
    void save(const ServerNode& node) override {
        savedNodes_.push_back(node);
    }
    // ...
};

// 测试应用层逻辑，无需真实数据库
TEST(IngestionServiceTest, ProcessHeartbeat) {
    MockNodeRepository mockRepo;
    IngestionService service(&mockRepo);

    HeartbeatRequestDTO dto = createTestDTO();
    auto response = service.processHeartbeat(dto);

    EXPECT_EQ(response.code, 0);
    EXPECT_EQ(mockRepo.savedNodes_.size(), 1);
}
```

### 4. 可维护性
- 更换JSON库？只需修改表现层的 `JsonUtils`
- 更换数据库？只需修改基础设施层的仓储实现
- 修改业务规则？只需修改领域层
- **各层独立演化，互不影响**

## 使用的设计模式

1. **DTO模式**: 跨层传输数据
2. **转换器模式**: DTO ↔ Domain 转换
3. **仓储模式**: 封装数据访问
4. **依赖注入**: 通过接口解耦

## 所需依赖库

- **JSON库**: `nlohmann/json` (推荐) 或 `RapidJSON`
- **HTTP框架**: `oatpp`, `Crow`, 或 `Pistache`
- **数据库驱动**: `libpqxx` (PostgreSQL), `influxdb-cpp` (InfluxDB)

## 下一步

1. 实现 `JsonUtils` 的具体解析逻辑（需要JSON库）
2. 创建 `IngestionService` 的完整实现
3. 实现基础设施层的仓储类
4. 在 `main.cpp` 中进行依赖注入组装
