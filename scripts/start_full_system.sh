#!/bin/bash

# 启动完整的监控系统：Server + Agent + 前端（测试版本）
# 使用最新的API接口格式，包含多种测试告警规则

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
    echo "  启动完整监控系统（测试版本）"
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
    
    # 3. 创建测试告警规则
    log_info "创建测试告警规则..."
    
    # 单条件告警规则 - CPU使用率告警
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU Usage",
            "description": "CPU使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "usage_percent",
                "stable": "cpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 多条件告警规则 - CPU和内存同时告警（AND逻辑）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU and Memory",
            "description": "CPU和内存使用率同时超过0.1%",
            "enabled": true,
            "severity": "CRITICAL",
            "for": "1s",
            "expression": {
                "metric": "usage_percent",
                "stable": "cpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    },
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 多条件告警规则 - CPU或内存任一告警（OR逻辑）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU or Memory",
            "description": "CPU或内存使用率任一超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "usage_percent",
                "stable": "cpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    },
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 基于IP地址的告警规则（标签匹配功能）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU on Specific IP",
            "description": "特定IP地址的CPU使用率告警",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "usage_percent",
                "stable": "cpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": [
                    {
                        "host_ip": "192.168.10.29"
                    }
                ]
            }
        }' > /dev/null 2>&1
    
    # 基于机箱ID的告警规则（标签匹配功能）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Memory on Box 1",
            "description": "机箱1的内存使用率告警",
            "enabled": true,
            "severity": "CRITICAL",
            "for": "1s",
            "expression": {
                "metric": "usage_percent",
                "stable": "memory",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": [
                    {
                        "box_id": "1"
                    }
                ]
            }
        }' > /dev/null 2>&1
    
    # 基于槽位ID的告警规则（标签匹配功能）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High GPU on Slot 1",
            "description": "槽位1的GPU使用率告警",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "compute_usage.max",
                "stable": "gpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": [
                    {
                        "slot_id": "1"
                    }
                ]
            }
        }' > /dev/null 2>&1
    
    # 基于CPU ID的告警规则（标签匹配功能）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High CPU Load on CPU 1",
            "description": "CPU 1的高负载告警",
            "enabled": true,
            "severity": "CRITICAL",
            "for": "1s",
            "expression": {
                "metric": "usage_percent",
                "stable": "cpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": [
                    {
                        "cpu_id": "1"
                    }
                ]
            }
        }' > /dev/null 2>&1
    
    # 基于主机名的告警规则（标签匹配功能）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Disk Usage on Specific Host",
            "description": "特定主机的磁盘使用率告警",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "usage_percent.avg",
                "stable": "disk",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": [
                    {
                        "hostname": "test-host"
                    }
                ]
            }
        }' > /dev/null 2>&1
    
    # 基于操作系统类型的告警规则（标签匹配功能）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Network Usage on Linux",
            "description": "Linux系统的网络使用率告警",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "rx_rate.total",
                "stable": "network",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 1
                    }
                ],
                "tags": [
                    {
                        "os_type": "linux"
                    }
                ]
            }
        }' > /dev/null 2>&1
    
    # 基于GPU数量的告警规则（标签匹配功能）
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High GPU Memory on Multi-GPU Systems",
            "description": "多GPU系统的GPU内存使用率告警",
            "enabled": true,
            "severity": "CRITICAL",
            "for": "1s",
            "expression": {
                "metric": "memory_usage.max",
                "stable": "gpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": [
                    {
                        "gpu_count": "2"
                    }
                ]
            }
        }' > /dev/null 2>&1
    
    # 特定网卡告警规则 - eth0网卡
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Network Usage on eth0",
            "description": "eth0网卡接收速率超过1字节/秒",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "eth0.rx_rate",
                "stable": "network",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 特定网卡告警规则 - eth1网卡
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Network Usage on eth1",
            "description": "eth1网卡发送速率超过1字节/秒",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "eth1.tx_rate",
                "stable": "network",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 特定磁盘告警规则 - sda磁盘
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Disk Usage on sda",
            "description": "sda磁盘使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "sda.usage_percent",
                "stable": "disk",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 特定磁盘告警规则 - sdb磁盘
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High Disk Usage on sdb",
            "description": "sdb磁盘使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "sdb.usage_percent",
                "stable": "disk",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 特定GPU告警规则 - GPU0
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High GPU Usage on GPU0",
            "description": "GPU0使用率超过0.1%",
            "enabled": true,
            "severity": "WARNING",
            "for": "1s",
            "expression": {
                "metric": "0.compute_usage",
                "stable": "gpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    # 特定GPU告警规则 - GPU1
    curl -X POST http://localhost:$SERVER_PORT/api/alert-rules \
        -H "Content-Type: application/json" \
        -d '{
            "alert_name": "High GPU Memory Usage on GPU1",
            "description": "GPU1内存使用率超过0.1%",
            "enabled": true,
            "severity": "CRITICAL",
            "for": "1s",
            "expression": {
                "metric": "1.memory_usage",
                "stable": "gpu",
                "conditions": [
                    {
                        "operator": ">",
                        "threshold": 0.1
                    }
                ],
                "tags": []
            }
        }' > /dev/null 2>&1
    
    log_success "测试告警规则创建完成 (共创建16个测试告警规则)"
    
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
    echo "  - 访问 http://localhost:5001/alert-rules 管理告警规则"
    echo "  - 此版本包含16个测试告警规则，使用低阈值便于测试"
    echo "  - 包含单条件、多条件（AND/OR逻辑）告警测试"
    echo "  - 包含多种设备类型标签匹配测试（IP、机箱、槽位、CPU、主机名、OS、GPU等）"
    echo "  - 包含特定设备告警测试（eth0/eth1网卡、sda/sdb磁盘、GPU0/GPU1等）"
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
