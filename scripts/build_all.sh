#!/bin/bash

# 统一编译脚本 - 编译server和agent到同一目录

set -e

# 获取脚本所在目录和项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

BUILD_DIR="$PROJECT_ROOT/build"
SERVER_DIR="$PROJECT_ROOT/server"
AGENT_DIR="$PROJECT_ROOT/agent"

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

# 清理函数
cleanup() {
    log_info "清理旧的构建目录..."
    rm -rf "$BUILD_DIR"
    rm -rf "$SERVER_DIR/build"
    rm -rf "build_agent"
    rm -rf "build_agent_test"
}

main() {
    echo "=========================================="
    echo "  统一编译 Server 和 Agent"
    echo "=========================================="
    
    # 1. 清理旧构建
    cleanup
    
    # 2. 创建统一构建目录
    log_info "创建统一构建目录: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    
    # 3. 编译 Server
    log_info "编译 Server..."
    cd "$SERVER_DIR"
    mkdir -p build
    cd build
    
    cmake ..
    make -j4
    
    # 复制server可执行文件到统一目录
    cp monitoring_server "$BUILD_DIR/"
    log_success "Server 编译完成"
    
    cd ../..
    
    # 4. 编译 Agent
    log_info "编译 Agent..."
    cd "$AGENT_DIR"
    mkdir -p build
    cd build
    
    cmake ..
    make -j4
    
    # 复制agent可执行文件到统一目录
    cp monitoring_agent "$BUILD_DIR/"
    log_success "Agent 编译完成"
    
    cd ../..
    
    # 5. 显示结果
    echo "=========================================="
    log_success "编译完成！"
    echo ""
    echo "可执行文件位置:"
    echo "  Server: $BUILD_DIR/monitoring_server"
    echo "  Agent:  $BUILD_DIR/monitoring_agent"
    echo ""
    echo "使用方法:"
    echo "  # 启动 Server"
    echo "  ./$BUILD_DIR/monitoring_server [port]"
    echo ""
    echo "  # 启动 Agent"
    echo "  ./$BUILD_DIR/monitoring_agent [server_host] [server_port]"
    echo ""
    echo "示例:"
    echo "  ./$BUILD_DIR/monitoring_server 18888"
    echo "  ./$BUILD_DIR/monitoring_agent localhost 18888"
    echo "=========================================="
}

main "$@"
