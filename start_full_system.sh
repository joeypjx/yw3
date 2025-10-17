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
    if [ ! -z "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
    fi
    if [ ! -z "$AGENT_PID" ]; then
        kill $AGENT_PID 2>/dev/null
        wait $AGENT_PID 2>/dev/null
    fi
    if [ ! -z "$FRONTEND_PID" ]; then
        kill $FRONTEND_PID 2>/dev/null
        wait $FRONTEND_PID 2>/dev/null
    fi
    log_success "清理完成"
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
    
    log_success "告警规则创建完成"
    
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
