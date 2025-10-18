# Server Monitoring System

一个基于C++实现的服务器集群资源监控软件，采用Agent-Server架构。

## 项目结构

```
yw3/
├── server/                      # 监控服务端
│   ├── domain/                  # 领域层 - 业务核心
│   │   ├── entities/           # 领域实体
│   │   ├── repositories/       # 仓储接口
│   │   └── services/           # 领域服务
│   ├── application/            # 应用层 - 用例编排
│   │   ├── services/           # 应用服务
│   │   └── dto/                # 数据传输对象
│   ├── infrastructure/         # 基础设施层
│   │   ├── repositories/       # 仓储实现
│   │   ├── adapters/           # HTTP适配器
│   │   ├── notifiers/          # 通知器
│   │   └── schedulers/         # 调度器
│   ├── presentation/           # 接口/表现层
│   │   ├── controllers/        # HTTP控制器
│   │   └── api/                # API路由定义
│   ├── common/                 # 通用工具
│   │   ├── id_generator.hpp   # ID生成器
│   │   └── time_utils.hpp     # 时间工具
│   ├── main.cpp               # 服务端入口 (组合根)
│   └── CMakeLists.txt
│
├── agent/                      # 监控代理
│   ├── collector/             # 数据采集
│   ├── formatter/             # 数据格式化
│   ├── reporter/              # 数据上报
│   ├── scheduler/             # 调度器
│   ├── config/                # 配置管理
│   ├── agent_main.cpp         # 代理入口
│   └── CMakeLists.txt
│
├── frontend/                   # Web前端
│   ├── monitoring_dashboard.py # Flask应用
│   ├── templates/              # HTML模板
│   ├── requirements.txt        # Python依赖
│   ├── start_frontend.sh      # 前端启动脚本
│   └── README.md
│
├── scripts/                    # 脚本文件
│   ├── build_all.sh           # 统一编译脚本
│   ├── start_full_system.sh   # 完整系统启动脚本
│   └── cleanup_processes.sh   # 进程清理脚本
│
├── config/                     # 配置文件
│   ├── node.yaml              # 节点配置
│   └── resource.yaml         # 资源配置
│
├── docs/                       # 项目文档
│   ├── ALERT_EVENTS_API.md    # 告警事件API文档
│   ├── ANALYSIS_SUMMARY.txt   # 分析总结
│   ├── agent_api_verification.md
│   ├── data_conversion_flow.md
│   ├── dual_tags_usage.md     # 双标签使用说明
│   ├── extended_metrics_documentation.md
│   ├── 快速参考指南.md
│   ├── 系统架构详解.md
│   ├── 项目结构分析.md
│   ├── 初始设计.md
│   └── README.md
│
├── tests/                      # 测试文件
├── build/                      # 编译输出目录
├── logs/                       # 日志文件目录
├── CMakeLists.txt             # 根CMake配置
├── start.sh                   # 项目启动脚本（快捷方式）
├── build.sh                   # 项目编译脚本（快捷方式）
├── cleanup.sh                 # 项目清理脚本（快捷方式）
└── README.md                  # 项目说明
```

## 核心组件

### Server端
- **领域层**: ServerNode, MetricSnapshot, AlertRule, AlertEvent
- **应用层**: IngestionService, QueryService, AlertingOrchestratorService
- **基础设施层**: PostgreSQL/InfluxDB仓储实现, HTTP适配器, 通知器
- **表现层**: HttpApiController (提供RESTful API), API路由定义

### Agent端
- **Collector**: 系统资源采集
- **Formatter**: 数据序列化
- **Reporter**: HTTP上报
- **Scheduler**: 定时调度

## 快速开始

### 1. 编译项目

```bash
# 使用快捷脚本编译
./build.sh

# 或者使用完整路径
./scripts/build_all.sh
```

### 2. 启动完整系统

```bash
# 使用快捷脚本启动（推荐）
./start.sh

# 或者使用完整路径
./scripts/start_full_system.sh
```

这将启动：
- 监控服务器（端口18888）
- 监控代理
- Web前端（端口5001）

### 3. 访问系统

- **Web前端**: http://localhost:5001
- **API接口**: http://localhost:18888/api/

### 4. 清理进程

```bash
# 使用快捷脚本清理
./cleanup.sh

# 或者使用完整路径
./scripts/cleanup_processes.sh
```

## 系统测试

```bash
# 运行系统测试
./test_system.sh

# 测试告警事件API
./test_alert_events.sh
```

## API文档

### 告警规则管理API

- `POST /api/alert-rules` - 创建告警规则
- `GET /api/alert-rules` - 获取告警规则列表
- `GET /api/alert-rules/{id}` - 获取告警规则详情
- `PUT /api/alert-rules/{id}` - 更新告警规则
- `DELETE /api/alert-rules/{id}` - 删除告警规则
- `POST /api/alert-rules/{id}/enable` - 启用告警规则
- `POST /api/alert-rules/{id}/disable` - 禁用告警规则
- `GET /api/alert-rules/active` - 获取启用的告警规则

### 告警事件管理API

- `GET /api/alert-events` - 获取所有告警事件
- `GET /api/alert-events/{id}` - 获取告警事件详情
- `GET /api/alert-events/node/{nodeId}` - 根据节点获取告警事件
- `GET /api/alert-events/rule/{ruleId}` - 根据规则获取告警事件
- `GET /api/alert-events/active` - 获取活跃的告警事件
- `GET /api/alert-events/status/{status}` - 根据状态获取告警事件
- `POST /api/alert-events/{id}/acknowledge` - 认知告警事件
- `POST /api/alert-events/{id}/resolve` - 解决告警事件

详细的API文档请参考：[告警事件API文档](ALERT_EVENTS_API.md)

## 前端界面

### 启动前端

```bash
# 方法1: 使用完整系统启动脚本（推荐）
./start_full_system.sh

# 方法2: 手动启动前端
cd frontend
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python3 monitoring_dashboard.py
```

### 访问前端

打开浏览器访问：http://localhost:5001

### 前端功能

- 📊 **实时仪表板**: 显示告警规则和事件的统计信息
- 🔄 **自动刷新**: 每5秒自动更新数据
- 🎯 **告警管理**: 支持认知和解决告警事件
- 📱 **响应式设计**: 支持桌面和移动设备
- 🎨 **现代化UI**: 使用Bootstrap 5和Font Awesome图标

### 演示脚本

```bash
# 运行前端演示
./demo_frontend.sh
```

## 技术栈

- C++17
- CMake 3.15+
- PostgreSQL (元数据存储)
- InfluxDB (时序数据存储)
- HTTP库 (待定: oatpp/Crow)
- JSON库 (nlohmann/json)

## 设计原则

本项目严格遵循:
- 分层架构
- 依赖倒置原则
- 领域驱动设计 (DDD)
- 高内聚、低耦合
