#!/bin/bash

# 大量节点数据上传测试脚本
# 用于测试系统在大量节点持续上传数据时的性能表现

set -e

PROJECT_ROOT="/Users/panjinxue/编程/yw3"
SERVER_PORT=18888
LOG_DIR="logs"

echo "=========================================="
echo "  大量节点数据上传测试"
echo "=========================================="

# 检查系统是否运行
if ! curl -s http://localhost:$SERVER_PORT/api/nodes > /dev/null 2>&1; then
    echo "❌ Server未运行，请先启动系统"
    exit 1
fi

echo "✅ Server运行正常"

# 创建测试节点数据
create_test_nodes() {
    echo "📝 创建测试节点..."
    
    # 创建100个测试节点
    for i in {1..100}; do
        node_id="test-node-$i"
        host_ip="192.168.1.$((100 + i))"
        
        curl -X POST http://localhost:$SERVER_PORT/heartbeat \
            -H "Content-Type: application/json" \
            -d "{
                \"api_version\": 1,
                \"data\": {
                    \"hostname\": \"$node_id\",
                    \"host_ip\": \"$host_ip\",
                    \"box_id\": $i,
                    \"slot_id\": $i,
                    \"cpu_id\": $i,
                    \"srio_id\": $i,
                    \"service_port\": $((8000 + i)),
                    \"box_type\": \"test-box\",
                    \"board_type\": \"test-board\",
                    \"cpu_type\": \"test-cpu\",
                    \"os_type\": \"linux\",
                    \"resource_type\": \"compute\",
                    \"cpu_arch\": \"x86_64\",
                    \"gpu\": [
                        {
                            \"index\": 0,
                            \"name\": \"GPU-$i\"
                        }
                    ]
                }
            }" > /dev/null 2>&1
    done
    
    echo "✅ 创建了100个测试节点"
}

# 模拟大量数据上传
simulate_massive_data_upload() {
    echo "📊 开始模拟大量数据上传..."
    
    local upload_count=0
    local batch_size=50
    local total_batches=200  # 总共上传 200 * 50 = 10000 条记录
    
    for batch in $(seq 1 $total_batches); do
        echo "📤 上传批次 $batch/$total_batches..."
        
        # 为每个节点生成数据
        for node_id in $(seq 1 100); do
            host_ip="192.168.1.$((100 + node_id))"
            node_id_name="test-node-$node_id"
            
            # 生成随机指标数据
            cpu_usage=$(echo "scale=1; $RANDOM/327.67" | bc -l | sed 's/^\./0./')
            memory_usage=$(echo "scale=1; $RANDOM/327.67" | bc -l | sed 's/^\./0./')
            disk_usage=$(echo "scale=1; $RANDOM/327.67" | bc -l | sed 's/^\./0./')
            gpu_usage=$(echo "scale=1; $RANDOM/327.67" | bc -l | sed 's/^\./0./')
            
            # 先发送心跳保持节点在线状态
            curl -X POST http://localhost:$SERVER_PORT/heartbeat \
                -H "Content-Type: application/json" \
                -d "{
                    \"api_version\": 1,
                    \"data\": {
                        \"hostname\": \"$node_id_name\",
                        \"host_ip\": \"$host_ip\",
                        \"box_id\": $node_id,
                        \"slot_id\": $node_id,
                        \"cpu_id\": $node_id,
                        \"srio_id\": $node_id,
                        \"service_port\": $((8000 + node_id)),
                        \"box_type\": \"test-box\",
                        \"board_type\": \"test-board\",
                        \"cpu_type\": \"test-cpu\",
                        \"os_type\": \"linux\",
                        \"resource_type\": \"compute\",
                        \"cpu_arch\": \"x86_64\",
                        \"gpu\": [
                            {
                                \"index\": 0,
                                \"name\": \"GPU-$node_id\"
                            }
                        ]
                    }
                }" > /dev/null 2>&1 &
            
            # 然后发送资源数据
            curl -X POST http://localhost:$SERVER_PORT/resource \
                -H "Content-Type: application/json" \
                -d "{
                    \"api_version\": 1,
                    \"data\": {
                        \"host_ip\": \"$host_ip\",
                        \"resource\": {
                            \"cpu\": {
                                \"usage_percent\": $cpu_usage,
                                \"load_avg_1m\": $(echo "scale=2; $RANDOM/32767" | bc -l | sed 's/^\./0./'),
                                \"load_avg_5m\": $(echo "scale=2; $RANDOM/32767" | bc -l | sed 's/^\./0./'),
                                \"load_avg_15m\": $(echo "scale=2; $RANDOM/32767" | bc -l | sed 's/^\./0./'),
                                \"core_count\": 8,
                                \"core_allocated\": 4,
                                \"temperature\": $(echo "scale=1; 40 + $RANDOM/327.67" | bc -l | sed 's/^\./0./'),
                                \"voltage\": 1.2,
                                \"current\": $(echo "scale=2; 10 + $RANDOM/327.67" | bc -l | sed 's/^\./0./'),
                                \"power\": $(echo "scale=1; 50 + $RANDOM/327.67" | bc -l | sed 's/^\./0./')
                            },
                            \"memory\": {
                                \"total\": 8589934592,
                                \"used\": $(echo "scale=0; 8589934592 * $memory_usage / 100" | bc -l),
                                \"free\": $(echo "scale=0; 8589934592 * (100 - $memory_usage) / 100" | bc -l),
                                \"usage_percent\": $memory_usage
                            },
                            \"disk\": [
                                {
                                    \"device\": \"/dev/sda1\",
                                    \"mount_point\": \"/\",
                                    \"total\": 107374182400,
                                    \"used\": $(echo "scale=0; 107374182400 * $disk_usage / 100" | bc -l),
                                    \"free\": $(echo "scale=0; 107374182400 * (100 - $disk_usage) / 100" | bc -l),
                                    \"usage_percent\": $disk_usage
                                }
                            ],
                            \"network\": [
                                {
                                    \"interface\": \"eth0\",
                                    \"rx_bytes\": $(echo "scale=0; $RANDOM * 1000" | bc -l),
                                    \"tx_bytes\": $(echo "scale=0; $RANDOM * 1000" | bc -l),
                                    \"rx_packets\": $RANDOM,
                                    \"tx_packets\": $RANDOM,
                                    \"rx_errors\": 0,
                                    \"tx_errors\": 0,
                                    \"rx_rate\": $(echo "scale=2; $RANDOM/327.67" | bc -l | sed 's/^\./0./'),
                                    \"tx_rate\": $(echo "scale=2; $RANDOM/327.67" | bc -l | sed 's/^\./0./')
                                }
                            ],
                            \"gpu\": [
                                {
                                    \"index\": 0,
                                    \"name\": \"GPU-$node_id\",
                                    \"compute_usage\": $gpu_usage,
                                    \"mem_usage\": $(echo "scale=1; $RANDOM/327.67" | bc -l | sed 's/^\./0./'),
                                    \"mem_used\": $(echo "scale=0; 8589934592 * $RANDOM / 32767" | bc -l),
                                    \"mem_total\": 8589934592,
                                    \"temperature\": $(echo "scale=1; 50 + $RANDOM/327.67" | bc -l | sed 's/^\./0./'),
                                    \"power\": $(echo "scale=1; 100 + $RANDOM/327.67" | bc -l | sed 's/^\./0./')
                                }
                            ],
                            \"gpu_allocated\": 1,
                            \"gpu_num\": 1
                        },
                        \"component\": []
                    }
                }" > /dev/null 2>&1 &
            
            upload_count=$((upload_count + 1))
            
            # 控制并发数量（每50个节点为一组）
            if [ $((node_id % batch_size)) -eq 0 ]; then
                wait  # 等待当前批次完成
                echo "   ✅ 已处理 $node_id 个节点（心跳+资源数据）"
            fi
        done
    done
    
    wait  # 等待所有后台任务完成
    echo "✅ 数据上传完成，总共处理了 $upload_count 个节点（每个节点发送心跳+资源数据）"
}

# 监控系统状态
monitor_system_status() {
    echo "📊 监控系统状态..."
    
    # 检查节点数量
    node_count=$(curl -s http://localhost:$SERVER_PORT/api/nodes | jq '.nodes | length' 2>/dev/null || echo "0")
    echo "📈 当前节点数量: $node_count"
    
    # 检查告警事件数量
    event_count=$(curl -s http://localhost:$SERVER_PORT/api/alert-events | jq '.events | length' 2>/dev/null || echo "0")
    echo "🚨 当前告警事件数量: $event_count"
    
    # 检查告警规则数量
    rule_count=$(curl -s http://localhost:$SERVER_PORT/api/alert-rules | jq '.rules | length' 2>/dev/null || echo "0")
    echo "📋 当前告警规则数量: $rule_count"
}

# 性能测试
performance_test() {
    echo "⚡ 开始性能测试..."
    
    local start_time=$(date +%s)
    
    # 测试查询性能
    echo "🔍 测试查询性能..."
    for i in {1..10}; do
        curl -s http://localhost:$SERVER_PORT/api/nodes > /dev/null 2>&1
        curl -s http://localhost:$SERVER_PORT/api/alert-events > /dev/null 2>&1
        curl -s http://localhost:$SERVER_PORT/api/alert-rules > /dev/null 2>&1
    done
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    echo "✅ 性能测试完成，耗时: ${duration}秒"
}

# 主函数
main() {
    echo "🚀 开始大量节点数据上传测试"
    echo "=========================================="
    
    # 创建测试节点
    create_test_nodes
    
    # 等待节点注册
    sleep 2
    
    # 监控初始状态
    monitor_system_status
    
    # 模拟大量数据上传
    simulate_massive_data_upload
    
    # 等待数据处理
    sleep 5
    
    # 监控最终状态
    echo ""
    echo "📊 最终系统状态:"
    monitor_system_status
    
    # 性能测试
    performance_test
    
    echo ""
    echo "=========================================="
    echo "✅ 大量节点数据上传测试完成！"
    echo "=========================================="
    echo "💡 提示:"
    echo "  - 系统已处理大量数据上传（心跳+资源数据）"
    echo "  - 每个节点都会先发送心跳保持在线状态"
    echo "  - 可以访问 http://localhost:5001 查看前端界面"
    echo "  - 检查日志文件了解详细处理情况"
    echo "  - 系统会自动清理过期数据以控制内存使用"
    echo "=========================================="
}

# 运行主函数
main "$@"
