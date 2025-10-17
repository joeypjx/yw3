# 监控系统前端

这是一个基于Flask的简单Web前端，用于显示监控系统的告警规则和事件数据。

## 功能特性

- 📊 **实时仪表板**: 显示告警规则和事件的统计信息
- 🔄 **自动刷新**: 每5秒自动刷新数据
- 🎯 **告警管理**: 支持认知和解决告警事件
- 📱 **响应式设计**: 支持桌面和移动设备
- 🎨 **现代化UI**: 使用Bootstrap 5和Font Awesome图标

## 安装和运行

### 方法1: 使用完整系统启动脚本（推荐）

```bash
# 在项目根目录运行
./start_full_system.sh
```

这将自动启动：
- 监控系统Server
- 监控Agent
- 前端Web界面

### 方法2: 手动启动

1. **安装Python依赖**:
```bash
cd frontend
pip3 install -r requirements.txt
```

2. **确保监控系统Server正在运行**:
```bash
# 在另一个终端中启动Server
./build/monitoring_server 18888
```

3. **启动前端**:
```bash
cd frontend
python3 monitoring_dashboard.py
```

4. **访问前端**:
打开浏览器访问 http://localhost:5001

## 界面说明

### 统计卡片
- **告警规则**: 显示总规则数和活跃规则数
- **告警事件**: 显示总事件数和活跃事件数
- **正在触发**: 显示FIRING状态的事件数
- **已解决**: 显示RESOLVED状态的事件数

### 告警规则列表
- 显示所有告警规则的详细信息
- 包括规则名称、指标、阈值、操作符等
- 显示规则状态（启用/禁用）和严重等级

### 告警事件列表
- 显示所有告警事件
- 支持按状态筛选（FIRING、ACKNOWLEDGED、RESOLVED）
- 提供认知和解决操作按钮
- 显示触发值、时间戳等详细信息

## API接口

前端通过以下API与监控系统通信：

- `GET /api/dashboard-data` - 获取仪表板数据
- `POST /api/acknowledge-event` - 认知告警事件
- `POST /api/resolve-event` - 解决告警事件

## 配置

可以在 `monitoring_dashboard.py` 中修改以下配置：

```python
SERVER_BASE_URL = "http://localhost:18888"  # 监控系统Server地址
REFRESH_INTERVAL = 5  # 自动刷新间隔（秒）
```

## 技术栈

- **后端**: Flask (Python)
- **前端**: HTML5 + CSS3 + JavaScript
- **UI框架**: Bootstrap 5
- **图标**: Font Awesome 6
- **HTTP客户端**: requests

## 浏览器支持

- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## 故障排除

### 前端无法连接Server
1. 确保监控系统Server正在运行 (http://localhost:18888)
2. 检查防火墙设置
3. 查看前端日志: `frontend.log`

### 数据不更新
1. 检查网络连接
2. 查看浏览器控制台错误
3. 确认Server API正常工作

### 页面显示异常
1. 清除浏览器缓存
2. 检查JavaScript控制台错误
3. 确保Bootstrap和Font Awesome资源加载正常

## 开发说明

### 添加新功能
1. 在 `monitoring_dashboard.py` 中添加新的API端点
2. 在 `dashboard.html` 中添加对应的前端界面
3. 更新JavaScript函数处理新功能

### 自定义样式
修改 `dashboard.html` 中的CSS样式部分，或创建独立的CSS文件。

### 添加新的API调用
在 `MonitoringAPI` 类中添加新的方法，然后在Flask路由中调用。
