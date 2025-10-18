#!/bin/bash

# 启动完整的监控系统：Server + Agent + 前端

SERVER_PORT=18888
LOG_DIR="full_system_logs"

# 颜色输出函数
log_info() {
    echo -e "\033[34m[INFO]\033[0m $1"
}

log_success() {
    echo -e "\033[32m[SUCCESS]\033[0m $1"
}

log_error() {
    echo -e "\033[31m[ERROR]\033[0m $1"
}

log_warning() {
    echo -e "\033[33m[WARNING]\033[0m $1"
}

# 清理函数
cleanup() {
    log_info "清理进程..."
    
    # 强制终止所有相关进程
    if [ ! -z "$SERVER_PID" ]; then
        kill -TERM $SERVER_PID 2>/dev/null
        kill -KILL $SERVER_PID 2>/dev/null
    fi
    if [ ! -z "$AGENT_PID" ]; then
        kill -TERM $AGENT_PID 2>/dev/null
        kill -KILL $AGENT_PID 2>/dev/null
    fi
    if [ ! -z "$FRONTEND_PID" ]; then
        kill -TERM $FRONTEND_PID 2>/dev/null
        kill -KILL $FRONTEND_PID 2>/dev/null
    fi
    
    # 清理可能遗留的进程
    pkill -f "monitoring_server" 2>/dev/null
    pkill -f "monitoring_agent" 2>/dev/null
    pkill -f "monitoring_dashboard.py" 2>/dev/null
    
    log_success "清理完成"
    exit 0
}

# 设置信号处理
trap cleanup EXIT INT TERM

main() {
    echo "=========================================="
    echo "  启动完整监控系统"
    echo "=========================================="
    
    # 1. 编译系统
    log_info "编译监控系统..."
    if ! ./build_all.sh; then
        log_error "编译失败"
        exit 1
    fi
    log_success "编译完成"
    
    # 2. 启动Server
    log_info "启动Server (端口: $SERVER_PORT)..."
    mkdir -p "$LOG_DIR"
    ./build/monitoring_server $SERVER_PORT > "$LOG_DIR/server.log" 2>&1 &
    SERVER_PID=$!
    
    # 等待Server启动
    sleep 5
    if curl -s http://localhost:$SERVER_PORT/ > /dev/null 2>&1; then
        log_success "Server启动成功"
    else
        log_error "Server启动失败"
        exit 1
    fi
    
    # 3. 创建告警规则
    log_info "创建告警规则..."
    
    # 基础CPU和内存告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "High CPU Usage",
            "metricName": "cpu.usage_percent",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "CPU使用率超过0.1%"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "High Memory Usage",
            "metricName": "memory.usage_percent",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "CRITICAL",
            "isEnabled": true,
            "description": "内存使用率超过0.1%"
        }' > /dev/null
    
    # 磁盘聚合告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "磁盘使用率平均值告警",
            "metricName": "disk.usage_percent.avg",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "磁盘使用率平均值超过0.1%"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "磁盘使用率最大值告警",
            "metricName": "disk.usage_percent.max",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "CRITICAL",
            "isEnabled": true,
            "description": "任意磁盘使用率超过0.1%"
        }' > /dev/null
    
    # 特定磁盘告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "根分区使用率告警",
            "metricName": "disk./dev/sda1.usage_percent",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "CRITICAL",
            "isEnabled": true,
            "description": "根分区使用率超过0.1%"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "数据分区使用率告警",
            "metricName": "disk./dev/sdb1.usage_percent",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "数据分区使用率超过0.1%"
        }' > /dev/null
    
    # 网络聚合告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "网络接收速率告警",
            "metricName": "network.rx_rate.total",
            "threshold": 1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "网络总接收速率超过1字节/秒"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "网络发送速率告警",
            "metricName": "network.tx_rate.total",
            "threshold": 1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "网络总发送速率超过1字节/秒"
        }' > /dev/null
    
    # 特定网络接口告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "eth0网卡接收速率告警",
            "metricName": "network.eth0.rx_rate",
            "threshold": 1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "eth0网卡接收速率超过1字节/秒"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "eth0网卡发送速率告警",
            "metricName": "network.eth0.tx_rate",
            "threshold": 1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "eth0网卡发送速率超过1字节/秒"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "wlan0网卡接收速率告警",
            "metricName": "network.wlan0.rx_rate",
            "threshold": 1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "wlan0网卡接收速率超过1字节/秒"
        }' > /dev/null
    
    # GPU聚合告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "GPU计算使用率平均值告警",
            "metricName": "gpu.compute_usage.avg",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "GPU计算使用率平均值超过0.1%"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "GPU温度最大值告警",
            "metricName": "gpu.temperature.max",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "CRITICAL",
            "isEnabled": true,
            "description": "GPU温度最大值超过0.1度"
        }' > /dev/null
    
    # 特定GPU告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "GPU0温度告警",
            "metricName": "gpu.0.temperature",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "CRITICAL",
            "isEnabled": true,
            "description": "GPU0温度超过0.1度"
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "ruleName": "GPU1计算使用率告警",
            "metricName": "gpu.1.compute_usage",
            "threshold": 0.1,
            "operator": ">",
            "durationSeconds": 1,
            "severity": "WARNING",
            "isEnabled": true,
            "description": "GPU1计算使用率超过0.1%"
        }' > /dev/null
    
    log_success "告警规则创建完成 (共创建16个规则)"
    
    # 4. 启动Agent
    log_info "启动Agent..."
    ./build/monitoring_agent localhost $SERVER_PORT > "$LOG_DIR/agent.log" 2>&1 &
    AGENT_PID=$!
    sleep 3
    log_success "Agent启动完成"
    
    # 5. 启动前端
    log_info "启动前端..."
    cd frontend
    
    # 检查虚拟环境是否存在，如果不存在则创建
    if [ ! -d "venv" ]; then
        log_info "创建Python虚拟环境..."
        python3 -m venv venv
        log_success "虚拟环境创建完成"
    fi
    
    # 激活虚拟环境并安装依赖
    log_info "激活虚拟环境并安装依赖..."
    source venv/bin/activate
    pip install -r requirements.txt > /dev/null 2>&1
    
    # 在虚拟环境中启动前端
    python3 monitoring_dashboard.py > "../$LOG_DIR/frontend.log" 2>&1 &
    FRONTEND_PID=$!
    cd ..
    sleep 3
    log_success "前端启动完成"
    
    # 6. 等待告警事件生成
    log_info "等待告警事件生成..."
    sleep 10
    
    # 检查告警事件
    log_info "检查告警事件..."
    ALERT_COUNT=$(curl -s http://localhost:$SERVER_PORT/api/alert-events | jq -r '.total // 0' 2>/dev/null || echo "0")
    # 确保ALERT_COUNT是数字
    if ! [[ "$ALERT_COUNT" =~ ^[0-9]+$ ]]; then
        ALERT_COUNT=0
    fi
    if [ "$ALERT_COUNT" -gt 0 ]; then
        log_success "已生成 $ALERT_COUNT 个告警事件"
    else
        log_info "暂无告警事件，系统正常运行"
    fi
    
    # 7. 显示系统信息
    echo ""
    echo "=========================================="
    log_success "监控系统启动完成！"
    echo "=========================================="
    echo "🌐 前端地址: http://localhost:5001"
    echo "🔧 Server地址: http://localhost:$SERVER_PORT"
    echo "📊 Agent状态: 运行中"
    echo ""
    echo "📁 日志文件: $LOG_DIR/"
    echo "  - Server日志: $LOG_DIR/server.log"
    echo "  - Agent日志: $LOG_DIR/agent.log"
    echo "  - 前端日志: $LOG_DIR/frontend.log"
    echo ""
    echo "💡 提示:"
    echo "  - 打开浏览器访问 http://localhost:5001 查看监控仪表板"
    echo "  - 按 Ctrl+C 停止所有服务"
    echo "  - 前端会自动刷新显示最新的告警信息"
    echo "=========================================="
    
    # 8. 等待用户中断
    log_info "系统运行中... (按 Ctrl+C 停止)"
    while true; do
        sleep 1
    done
}

main "$@"
