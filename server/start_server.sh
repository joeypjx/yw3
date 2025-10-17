#!/bin/bash

# 监控服务器启动脚本
# 将输出同时显示在终端和保存到日志文件

set -e

# 配置
SERVER_EXECUTABLE="./monitoring_server"
LOG_FILE="server.log"
PORT=${1:-18888}

echo "=========================================="
echo "Starting Monitoring Server with Logging"
echo "=========================================="
echo "Port: $PORT"
echo "Log file: $LOG_FILE"
echo "=========================================="

# 创建日志目录（如果不存在）
mkdir -p logs

# 启动服务器，使用tee命令同时输出到终端和文件
# -a 表示追加模式
# 2>&1 表示将stderr也重定向到stdout
exec $SERVER_EXECUTABLE $PORT 2>&1 | tee -a logs/$LOG_FILE
