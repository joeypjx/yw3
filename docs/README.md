# 监控系统项目文档索引

## 快速开始

如果你是第一次接触这个项目，请按以下顺序阅读：

1. **项目结构分析.md** (首先读这个)
   - 了解项目的整体结构
   - 理解分层架构
   - 掌握核心模块

2. **系统架构详解.md** (然后读这个)
   - 查看详细的架构图
   - 理解数据流向
   - 掌握业务流程

3. **快速参考指南.md** (需要时查这个)
   - API端点速查表
   - 常见问题排查
   - 开发工作流

---

## 文档导航

### 结构和架构文档

| 文档名称 | 大小 | 内容描述 |
|---------|------|--------|
| [项目结构分析.md](项目结构分析.md) | 16KB | 项目概览、目录结构、分层架构、前端/Agent、构建系统、业务流程 |
| [系统架构详解.md](系统架构详解.md) | 33KB | 架构图解、流程图、状态机、数据流、实际案例 |
| [快速参考指南.md](快速参考指南.md) | 10KB | 速查表、编译运行、API参考、问题排查、开发指南 |

### 功能文档

| 文档名称 | 大小 | 内容描述 |
|---------|------|--------|
| [agent_api_verification.md](agent_api_verification.md) | 4.1KB | Agent API 验证和测试 |
| [data_conversion_flow.md](data_conversion_flow.md) | 6.9KB | 数据转换流程说明 |
| [dual_tags_usage.md](dual_tags_usage.md) | 4.5KB | 标签使用方法 |
| [extended_metrics_documentation.md](extended_metrics_documentation.md) | 6.6KB | 指标扩展文档 |

---

## 文档内容概览

### 项目结构分析.md (537行)

**适合场景**: 新开发人员入职、架构审查、项目规划

**主要内容**:
- 项目概览 (规模、技术栈、功能)
- 详细目录结构 (树形展示)
- 分层架构详解 (Domain/Application/Infrastructure/Presentation)
- 前端架构 (技术栈、应用结构)
- Agent代理架构 (模块划分)
- 构建系统说明 (CMake配置)
- 核心业务流程 (数据流、告警流程、事件生命周期)
- 设计原则 (DDD、DIP、工厂模式、状态机)
- 代码质量指标
- 当前开发状态 (已完成、待完成)
- 技术债和改进建议

**关键节点**:
- 分层架构如何组织: 第3节
- 核心业务流程: 第8节
- 待完成项目: 第11.2节

---

### 系统架构详解.md (540行)

**适合场景**: 代码复审、架构讨论、故障排查

**主要内容**:
- 系统整体架构图 (Agent → Server → Frontend)
- 分层架构详细图 (依赖关系)
- 告警规则评估流程图 (详细步骤)
- 告警事件生命周期状态机 (FIRING → ACKNOWLEDGED → RESOLVED)
- 依赖注入和组件关系图 (main.cpp中的初始化)
- API数据流示例 (Agent上报数据)
- 告警触发和事件生成示例 (完整场景)

**关键图表**:
- 整体架构: 第1节
- 告警评估流程: 第3节
- 依赖注入: 第5节

---

### 快速参考指南.md (419行)

**适合场景**: 日常开发、快速查询、问题解决

**主要内容**:
1. 项目快速导航
2. 核心文件速查表
3. 编译和运行指南
4. API端点速查表
5. 关键类和接口代码
6. 业务流程速览
7. 配置位置
8. 测试脚本
9. 常见问题排查流程
10. 代码质量工具
11. 开发工作流 (添加新功能步骤)
12. 性能参考数据
13. 文档导航
14. 快速问题排查流程图
15. 贡献指南

**速查表**:
- 文件位置表: 第2节
- API端点表: 第4节
- 常见问题: 第9节

---

### 其他功能文档

#### agent_api_verification.md
- Agent API验证方法
- 测试场景
- 预期结果

#### data_conversion_flow.md
- 数据转换过程
- 格式转换
- 流程说明

#### dual_tags_usage.md
- 标签系统说明
- 使用方法
- 应用场景

#### extended_metrics_documentation.md
- 指标扩展指南
- 支持的指标列表
- 扩展步骤

---

## 项目文档体系

```
监控系统文档
├── 入门文档
│   ├── README.md (项目主文档)
│   └── docs/项目结构分析.md (结构详解)
│
├── 架构文档
│   ├── docs/系统架构详解.md (架构图解)
│   └── docs/快速参考指南.md (参考手册)
│
├── 功能文档
│   ├── docs/agent_api_verification.md
│   ├── docs/data_conversion_flow.md
│   ├── docs/dual_tags_usage.md
│   └── docs/extended_metrics_documentation.md
│
├── API文档
│   └── ALERT_EVENTS_API.md (API详细文档)
│
└── 分析报告
    └── ANALYSIS_SUMMARY.txt (完整分析报告)
```

---

## 按需求查找文档

### 如果你想...

**... 了解项目整体结构**
→ 阅读 [项目结构分析.md](项目结构分析.md) 第1-2节

**... 理解数据流向**
→ 查看 [系统架构详解.md](系统架构详解.md) 第1、6节

**... 学习告警规则如何评估**
→ 查看 [系统架构详解.md](系统架构详解.md) 第3节

**... 查找某个API**
→ 查看 [快速参考指南.md](快速参考指南.md) 第4节

**... 添加新的告警规则类型**
→ 查看 [快速参考指南.md](快速参考指南.md) 第11节

**... 排查告警没有触发的问题**
→ 查看 [快速参考指南.md](快速参考指南.md) 第14节

**... 编译和运行系统**
→ 查看 [快速参考指南.md](快速参考指南.md) 第3节

**... 了解系统的设计原则**
→ 查看 [项目结构分析.md](项目结构分析.md) 第9节

**... 获取完整的分析报告**
→ 查看 [../ANALYSIS_SUMMARY.txt](../ANALYSIS_SUMMARY.txt)

---

## 文档维护

### 文档更新时机

这些文档应该在以下情况下更新:

1. 添加新的应用服务
   - 更新项目结构分析.md的第3.2节
   - 更新系统架构详解.md的依赖注入图

2. 修改API端点
   - 更新快速参考指南.md第4节
   - 更新API详细文档

3. 改变业务流程
   - 更新系统架构详解.md的流程图
   - 更新项目结构分析.md的第8节

4. 更新构建系统
   - 更新快速参考指南.md第3节
   - 更新项目结构分析.md第6节

### 文档命名规范

中文文档使用：`名称.md` (例如: 项目结构分析.md)
英文文档使用：`name_description.md` (例如: data_conversion_flow.md)

---

## 文档生成工具

### Markdown转PDF

```bash
# 使用pandoc转换为PDF
pandoc 项目结构分析.md -o 项目结构分析.pdf

# 使用pandoc转换为HTML
pandoc 项目结构分析.md -o 项目结构分析.html -s
```

### 生成目录

大多数Markdown编辑器 (VSCode, GitHub等) 会自动生成目录。

手动生成可使用:
```bash
# 使用markdown-toc
npm install -g markdown-toc
markdown-toc 项目结构分析.md > toc.txt
```

---

## 相关资源

### 源代码位置

- Server: `/Users/panjinxue/编程/yw3/server/`
- Agent: `/Users/panjinxue/编程/yw3/agent/`
- Frontend: `/Users/panjinxue/编程/yw3/frontend/`

### 重要命令

```bash
# 编译
./build_all.sh

# 启动完整系统
./start_full_system.sh

# 启动个别组件
./build/monitoring_server 18888
./build/monitoring_agent localhost 18888
cd frontend && python3 monitoring_dashboard.py
```

### 在线资源

- README.md: `/Users/panjinxue/编程/yw3/README.md`
- API文档: `/Users/panjinxue/编程/yw3/ALERT_EVENTS_API.md`
- 分析报告: `/Users/panjinxue/编程/yw3/ANALYSIS_SUMMARY.txt`

---

## 文档修订历史

| 日期 | 文档 | 变更 |
|------|------|------|
| 2025-10-18 | 所有 | 初次生成 |

---

## 反馈和改进

如果您发现文档有以下问题，请更新:

- 过时的信息
- 不准确的描述
- 缺失的内容
- 格式问题

请在修改后更新此README.md的修订历史。

---

生成时间: 2025-10-18 UTC
