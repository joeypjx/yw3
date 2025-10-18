#!/bin/bash

# 进程清理脚本
# 用于清理监控系统相关的进程

echo "正在清理监控系统进程..."

# 清理服务器进程
echo "清理监控服务器进程..."
pkill -f "monitoring_server" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✓ 监控服务器进程已清理"
else
    echo "ℹ 未找到监控服务器进程"
fi

# 清理代理进程
echo "清理监控代理进程..."
pkill -f "monitoring_agent" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✓ 监控代理进程已清理"
else
    echo "ℹ 未找到监控代理进程"
fi

# 清理前端进程
echo "清理监控前端进程..."
pkill -f "monitoring_dashboard" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✓ 监控前端进程已清理"
else
    echo "ℹ 未找到监控前端进程"
fi

# 清理Flask进程
echo "清理Flask进程..."
pkill -f "flask" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✓ Flask进程已清理"
else
    echo "ℹ 未找到Flask进程"
fi

# 清理Python进程（监控相关）
echo "清理Python监控进程..."
pkill -f "python.*monitoring" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✓ Python监控进程已清理"
else
    echo "ℹ 未找到Python监控进程"
fi

# 等待进程完全退出
echo "等待进程完全退出..."
sleep 2

# 强制清理（如果还有残留进程）
echo "强制清理残留进程..."
pkill -9 -f "monitoring_server" 2>/dev/null
pkill -9 -f "monitoring_agent" 2>/dev/null
pkill -9 -f "monitoring_dashboard" 2>/dev/null
pkill -9 -f "flask" 2>/dev/null

echo "进程清理完成！"
echo "可以使用以下命令检查是否还有相关进程："
echo "ps aux | grep -E '(monitoring|flask)' | grep -v grep"
