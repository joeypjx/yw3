#!/bin/bash

# 清理监控系统所有进程的脚本

echo "=========================================="
echo "  清理监控系统进程"
echo "=========================================="

# 颜色输出函数
log_info() {
    echo -e "\033[34m[INFO]\033[0m $1"
}

log_success() {
    echo -e "\033[32m[SUCCESS]\033[0m $1"
}

log_warning() {
    echo -e "\033[33m[WARNING]\033[0m $1"
}

# 清理函数
cleanup_processes() {
    log_info "查找监控系统相关进程..."
    
    # 查找所有相关进程
    local server_pids=$(ps aux | grep -E "monitoring_server" | grep -v grep | awk '{print $2}')
    local agent_pids=$(ps aux | grep -E "monitoring_agent" | grep -v grep | awk '{print $2}')
    local frontend_pids=$(ps aux | grep -E "python.*monitoring_dashboard" | grep -v grep | awk '{print $2}')
    local python_pids=$(ps aux | grep -E "python.*monitoring" | grep -v grep | awk '{print $2}')
    
    local total_killed=0
    
    # 清理Server进程
    if [ ! -z "$server_pids" ]; then
        log_info "清理Server进程: $server_pids"
        echo $server_pids | xargs kill -TERM 2>/dev/null
        sleep 2
        echo $server_pids | xargs kill -KILL 2>/dev/null
        total_killed=$((total_killed + $(echo $server_pids | wc -w)))
    fi
    
    # 清理Agent进程
    if [ ! -z "$agent_pids" ]; then
        log_info "清理Agent进程: $agent_pids"
        echo $agent_pids | xargs kill -TERM 2>/dev/null
        sleep 2
        echo $agent_pids | xargs kill -KILL 2>/dev/null
        total_killed=$((total_killed + $(echo $agent_pids | wc -w)))
    fi
    
    # 清理前端进程
    if [ ! -z "$frontend_pids" ]; then
        log_info "清理前端进程: $frontend_pids"
        echo $frontend_pids | xargs kill -TERM 2>/dev/null
        sleep 2
        echo $frontend_pids | xargs kill -KILL 2>/dev/null
        total_killed=$((total_killed + $(echo $frontend_pids | wc -w)))
    fi
    
    # 清理其他Python监控进程
    if [ ! -z "$python_pids" ]; then
        log_info "清理Python监控进程: $python_pids"
        echo $python_pids | xargs kill -TERM 2>/dev/null
        sleep 2
        echo $python_pids | xargs kill -KILL 2>/dev/null
        total_killed=$((total_killed + $(echo $python_pids | wc -w)))
    fi
    
    if [ $total_killed -gt 0 ]; then
        log_success "已清理 $total_killed 个进程"
    else
        log_info "没有找到需要清理的进程"
    fi
}

# 检查端口占用
check_ports() {
    log_info "检查端口占用情况..."
    
    local server_port=$(lsof -i :18888 2>/dev/null | wc -l)
    local frontend_port=$(lsof -i :5001 2>/dev/null | wc -l)
    
    if [ $server_port -gt 1 ]; then
        log_warning "端口18888仍被占用"
        lsof -i :18888
    else
        log_success "端口18888已释放"
    fi
    
    if [ $frontend_port -gt 1 ]; then
        log_warning "端口5001仍被占用"
        lsof -i :5001
    else
        log_success "端口5001已释放"
    fi
}

# 清理日志文件
cleanup_logs() {
    log_info "清理日志文件..."
    
    if [ -d "full_system_logs" ]; then
        rm -rf full_system_logs
        log_success "已清理 full_system_logs 目录"
    fi
    
    if [ -d "alert_flow_test_logs" ]; then
        rm -rf alert_flow_test_logs
        log_success "已清理 alert_flow_test_logs 目录"
    fi
    
    if [ -d "quick_alert_test_logs" ]; then
        rm -rf quick_alert_test_logs
        log_success "已清理 quick_alert_test_logs 目录"
    fi
    
    if [ -d "long_alert_test_logs" ]; then
        rm -rf long_alert_test_logs
        log_success "已清理 long_alert_test_logs 目录"
    fi
}

# 主函数
main() {
    cleanup_processes
    echo ""
    check_ports
    echo ""
    cleanup_logs
    echo ""
    echo "=========================================="
    log_success "清理完成！"
    echo "=========================================="
    echo "所有监控系统进程已停止"
    echo "相关端口已释放"
    echo "临时日志文件已清理"
    echo "=========================================="
}

main "$@"
