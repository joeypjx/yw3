#!/bin/bash

# 演示前端功能的脚本

echo "=========================================="
echo "  监控系统前端演示"
echo "=========================================="

# 检查系统状态
echo "检查系统状态..."

# 检查Server
if curl -s http://localhost:18888/api/alert-events > /dev/null 2>&1; then
    echo "✅ 监控系统Server正在运行 (http://localhost:18888)"
else
    echo "❌ 监控系统Server未运行，请先启动Server"
    exit 1
fi

# 检查前端
if curl -s http://localhost:5001/api/dashboard-data > /dev/null 2>&1; then
    echo "✅ 前端服务正在运行 (http://localhost:5001)"
else
    echo "❌ 前端服务未运行，请先启动前端"
    exit 1
fi

echo ""
echo "=========================================="
echo "  系统状态概览"
echo "=========================================="

# 获取系统数据
echo "获取系统数据..."

# 告警规则
echo ""
echo "📊 告警规则:"
curl -s http://localhost:18888/api/alert-rules | jq '.total' 2>/dev/null || echo "无法获取"

# 告警事件
echo ""
echo "🚨 告警事件:"
curl -s http://localhost:18888/api/alert-events | jq '.total' 2>/dev/null || echo "无法获取"

# 活跃告警事件
echo ""
echo "🔥 活跃告警事件:"
curl -s http://localhost:18888/api/alert-events/active | jq '.total' 2>/dev/null || echo "无法获取"

echo ""
echo "=========================================="
echo "  前端功能演示"
echo "=========================================="

echo "🌐 前端地址: http://localhost:5001"
echo ""
echo "📱 功能特性:"
echo "  - 实时监控仪表板"
echo "  - 告警规则管理"
echo "  - 告警事件查看"
echo "  - 告警事件操作（认知/解决）"
echo "  - 自动刷新（每5秒）"
echo ""
echo "💡 使用说明:"
echo "  1. 打开浏览器访问 http://localhost:5001"
echo "  2. 查看实时监控数据"
echo "  3. 对告警事件进行认知或解决操作"
echo "  4. 观察数据的自动刷新"
echo ""
echo "🔧 API测试:"
echo "  前端API地址: http://localhost:5001/api/dashboard-data"
echo "  监控API地址: http://localhost:18888/api/alert-events"
echo ""

# 显示最新的告警事件详情
echo "📋 最新告警事件详情:"
curl -s http://localhost:5001/api/dashboard-data | jq '.alert_events.events[0]' 2>/dev/null || echo "暂无告警事件"

echo ""
echo "=========================================="
echo "  演示完成！"
echo "=========================================="
