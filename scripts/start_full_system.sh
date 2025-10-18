#!/bin/bash

# 启动完整的监控系统：Server + Agent + 前端

# 获取脚本所在目录和项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

SERVER_PORT=18888
LOG_DIR="logs"

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
    if ! "$SCRIPT_DIR/build_all.sh"; then
        log_error "编译失败"
        exit 1
    fi
    log_success "编译完成"
    
    # 2. 启动Server
    log_info "启动Server (端口: $SERVER_PORT)..."
    mkdir -p "$PROJECT_ROOT/$LOG_DIR"
    "$PROJECT_ROOT/build/monitoring_server" $SERVER_PORT > "$PROJECT_ROOT/$LOG_DIR/server.log" 2>&1 &
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
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU Usage",
            "description": "CPU使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "cpu.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Memory Usage",
            "description": "内存使用率超过0.1%",
            "enabled": true,
            "severity": "CRITICAL",
            "expression": {
                "conditions": [
                    {
                        "metric": "memory.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 磁盘聚合告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Disk Usage (Aggregated)",
            "description": "磁盘使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "disk.usage_percent.avg",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 指定磁盘告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Disk Usage (sda)",
            "description": "sda磁盘使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "disk.sda.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Disk Usage (sdb)",
            "description": "sdb磁盘使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "disk.sdb.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 网络聚合告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Network Usage (Aggregated)",
            "description": "网络使用率超过1字节/秒",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "network.rx_rate.total",
                        "operator": ">",
                        "threshold": 1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 指定网卡告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Network Usage (eth0)",
            "description": "eth0网卡接收速率超过1字节/秒",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "network.eth0.rx_rate",
                        "operator": ">",
                        "threshold": 1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Network Usage (eth1)",
            "description": "eth1网卡发送速率超过1字节/秒",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "network.eth1.tx_rate",
                        "operator": ">",
                        "threshold": 1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # GPU聚合告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High GPU Usage (Aggregated)",
            "description": "GPU使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "gpu.compute_usage.max",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 指定GPU告警规则
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High GPU Usage (GPU0)",
            "description": "GPU0使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "gpu.0.compute_usage",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 多条件告警规则 - CPU和内存同时告警
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU and Memory Usage",
            "description": "CPU和内存使用率同时超过0.1%",
            "enabled": true,
            "severity": "CRITICAL",
            "expression": {
                "conditions": [
                    {
                        "metric": "cpu.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    },
                    {
                        "metric": "memory.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 多条件告警规则 - CPU或内存任一告警
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU or Memory Usage",
            "description": "CPU或内存使用率任一超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "expression": {
                "conditions": [
                    {
                        "metric": "cpu.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    },
                    {
                        "metric": "memory.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "OR"
            }
        }' > /dev/null
    
    # 多条件告警规则 - 磁盘和网络同时告警
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Disk and Network Usage",
            "description": "磁盘和网络使用率同时超过阈值",
            "enabled": true,
            "severity": "CRITICAL",
            "expression": {
                "conditions": [
                    {
                        "metric": "disk.usage_percent.avg",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    },
                    {
                        "metric": "network.rx_rate.total",
                        "operator": ">",
                        "threshold": 1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 多条件告警规则 - 复杂条件组合
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "System Resource Stress",
            "description": "系统资源压力告警：CPU>0.1% 且 (内存>0.1% 或 磁盘>0.1%)",
            "enabled": true,
            "severity": "CRITICAL",
            "expression": {
                "conditions": [
                    {
                        "metric": "cpu.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    },
                    {
                        "metric": "memory.usage_percent",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    },
                    {
                        "metric": "disk.usage_percent.avg",
                        "operator": ">",
                        "threshold": 0.1,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    # 低阈值告警规则 - 用于测试
    curl -X POST http://localhost:$SERVER_PORT/api/alarm/rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "Low Threshold Test",
            "description": "低阈值测试告警",
            "enabled": true,
            "severity": "INFO",
            "expression": {
                "conditions": [
                    {
                        "metric": "cpu.usage_percent",
                        "operator": ">",
                        "threshold": 0.01,
                        "duration": "1s"
                    }
                ],
                "logic": "AND"
            }
        }' > /dev/null
    
    log_success "告警规则创建完成 (共创建16个告警规则)"
    
    # 4. 启动Agent
    log_info "启动Agent..."
    "$PROJECT_ROOT/build/monitoring_agent" localhost $SERVER_PORT > "$PROJECT_ROOT/$LOG_DIR/agent.log" 2>&1 &
    AGENT_PID=$!
    sleep 3
    log_success "Agent启动完成"
    
    # 5. 启动前端
    log_info "启动前端..."
    cd "$PROJECT_ROOT/frontend"
    
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
    python3 monitoring_dashboard.py > "$PROJECT_ROOT/$LOG_DIR/frontend.log" 2>&1 &
    FRONTEND_PID=$!
    cd "$PROJECT_ROOT"
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
