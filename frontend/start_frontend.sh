#!/bin/bash

# 启动监控系统前端

echo "=========================================="
echo "  启动监控系统前端"
echo "=========================================="

# 检查Python是否安装
if ! command -v python3 &> /dev/null; then
    echo "错误: 未找到Python3，请先安装Python3"
    exit 1
fi

# 检查pip是否安装
if ! command -v pip3 &> /dev/null; then
    echo "错误: 未找到pip3，请先安装pip3"
    exit 1
fi

# 安装依赖
echo "安装Python依赖..."
pip3 install -r requirements.txt

# 启动前端服务
echo "启动前端服务..."
echo "前端地址: http://localhost:5001"
echo "请确保监控系统Server正在运行 (http://localhost:18888)"
echo "=========================================="

python3 monitoring_dashboard.py
