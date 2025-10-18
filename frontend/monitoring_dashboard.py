#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
监控系统前端仪表板
使用Flask和Bootstrap创建简单的Web界面
"""

import requests
import json
import time
from datetime import datetime
from flask import Flask, render_template, jsonify, request
import threading

app = Flask(__name__)

# 配置
SERVER_BASE_URL = "http://localhost:18888"
REFRESH_INTERVAL = 5  # 自动刷新间隔（秒）

class MonitoringAPI:
    """监控系统API客户端"""
    
    def __init__(self, base_url):
        self.base_url = base_url
    
    def get_alert_rules(self):
        """获取告警规则列表"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-rules", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_active_alert_rules(self):
        """获取活跃的告警规则"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-rules/active", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_alert_events(self):
        """获取告警事件列表"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-events", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_active_alert_events(self):
        """获取活跃的告警事件"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-events/active", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_alert_events_by_status(self, status):
        """根据状态获取告警事件"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-events/status/{status}", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def acknowledge_alert_event(self, event_id, operator_id="admin"):
        """认知告警事件"""
        try:
            response = requests.post(
                f"{self.base_url}/api/alert-events/{event_id}/acknowledge",
                json={"operatorId": operator_id},
                timeout=5
            )
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def resolve_alert_event(self, event_id):
        """解决告警事件"""
        try:
            response = requests.post(f"{self.base_url}/api/alert-events/{event_id}/resolve", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_nodes(self):
        """获取所有节点"""
        try:
            response = requests.get(f"{self.base_url}/api/nodes", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_node_metrics(self, node_id, recent_seconds=300):
        """获取节点指标数据"""
        try:
            response = requests.get(f"{self.base_url}/api/nodes/{node_id}/metrics?recentSeconds={recent_seconds}", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_node_metrics_by_time_range(self, node_id, start_time, end_time):
        """获取节点指定时间范围内的指标数据"""
        try:
            response = requests.get(
                f"{self.base_url}/api/nodes/{node_id}/metrics?startTime={start_time}&endTime={end_time}", 
                timeout=5
            )
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def create_alert_rule(self, rule_data):
        """创建告警规则"""
        try:
            response = requests.post(
                f"{self.base_url}/api/alert-rules",
                json=rule_data,
                timeout=5
            )
            return response.json() if response.status_code in [200, 201] else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def update_alert_rule(self, rule_id, rule_data):
        """更新告警规则"""
        try:
            response = requests.put(
                f"{self.base_url}/api/alert-rules/{rule_id}",
                json=rule_data,
                timeout=5
            )
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def delete_alert_rule(self, rule_id):
        """删除告警规则"""
        try:
            response = requests.delete(f"{self.base_url}/api/alert-rules/{rule_id}", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_alert_rule(self, rule_id):
        """获取单个告警规则"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-rules/{rule_id}", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_alert_events(self):
        """获取所有告警事件"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-events", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def acknowledge_alert_event(self, event_id, operator_id="admin"):
        """认知告警事件"""
        try:
            response = requests.post(
                f"{self.base_url}/api/alert-events/{event_id}/acknowledge",
                json={"operatorId": operator_id},
                timeout=5
            )
            return response.json() if response.status_code in [200, 201] else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def resolve_alert_event(self, event_id):
        """解决告警事件"""
        try:
            response = requests.post(f"{self.base_url}/api/alert-events/{event_id}/resolve", timeout=5)
            return response.json() if response.status_code in [200, 201] else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_active_alert_events(self):
        """获取活跃的告警事件"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-events/active", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_alert_events_by_status(self, status):
        """根据状态获取告警事件"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-events/status/{status}", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}
    
    def get_active_alert_rules(self):
        """获取活跃的告警规则"""
        try:
            response = requests.get(f"{self.base_url}/api/alert-rules/active", timeout=5)
            return response.json() if response.status_code == 200 else {"code": -1, "message": "API调用失败"}
        except Exception as e:
            return {"code": -1, "message": f"连接错误: {str(e)}"}

# 创建API客户端实例
api = MonitoringAPI(SERVER_BASE_URL)

@app.route('/')
def index():
    """主页面"""
    return render_template('dashboard.html')

@app.route('/alert-rules')
def alert_rules():
    """告警规则管理页面"""
    return render_template('alert_rules.html')

@app.route('/node/<node_id>')
def node_detail(node_id):
    """节点详情页面"""
    return render_template('node_detail.html', node_id=node_id)

@app.route('/api/node/<node_id>/metrics-history')
def get_node_metrics_history(node_id):
    """获取节点历史指标数据"""
    try:
        # 获取时间范围参数
        time_range = request.args.get('range', '1h')  # 默认1小时
        end_time = int(time.time())  # 当前时间
        
        # 根据时间范围计算开始时间
        if time_range == '1h':
            start_time = end_time - 3600  # 1小时前
        elif time_range == '6h':
            start_time = end_time - 21600  # 6小时前
        elif time_range == '24h':
            start_time = end_time - 86400  # 24小时前
        elif time_range == '7d':
            start_time = end_time - 604800  # 7天前
        else:
            start_time = end_time - 3600  # 默认1小时
        
        # 获取节点基本信息
        nodes = api.get_nodes()
        node_info = None
        if nodes.get("code") == 0 and nodes.get("nodes"):
            for node in nodes["nodes"]:
                if node["nodeId"] == node_id:
                    node_info = node
                    break
        
        if not node_info:
            return jsonify({"code": -1, "message": "节点不存在"}), 404
        
        # 获取指标数据
        metrics = api.get_node_metrics_by_time_range(node_id, start_time, end_time)
        
        return jsonify({
            "code": 0,
            "node": node_info,
            "metrics": metrics,
            "timeRange": time_range,
            "startTime": start_time,
            "endTime": end_time
        })
    except Exception as e:
        return jsonify({"code": -1, "message": str(e)}), 500

@app.route('/api/dashboard-data')
def get_dashboard_data():
    """获取仪表板数据"""
    try:
        # 获取各种数据
        alert_rules = api.get_alert_rules()
        active_alert_rules = api.get_active_alert_rules()
        alert_events = api.get_alert_events()
        active_alert_events = api.get_active_alert_events()
        firing_events = api.get_alert_events_by_status("firing")
        acknowledged_events = api.get_alert_events_by_status("acknowledged")
        resolved_events = api.get_alert_events_by_status("resolved")
        
        # 获取节点和指标数据
        nodes = api.get_nodes()
        
        # 为每个节点获取最近的指标数据
        node_metrics = {}
        if nodes.get("code") == 0 and nodes.get("nodes"):
            for node in nodes["nodes"]:
                node_id = node["nodeId"]
                metrics = api.get_node_metrics(node_id, 60)  # 最近1分钟的数据
                node_metrics[node_id] = metrics
        
        return jsonify({
            "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "alert_rules": alert_rules,
            "active_alert_rules": active_alert_rules,
            "alert_events": alert_events,
            "active_alert_events": active_alert_events,
            "firing_events": firing_events,
            "acknowledged_events": acknowledged_events,
            "resolved_events": resolved_events,
            "nodes": nodes,
            "node_metrics": node_metrics
        })
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/api/acknowledge-event', methods=['POST'])
def acknowledge_event():
    """认知告警事件"""
    try:
        data = request.get_json()
        event_id = data.get('eventId')
        operator_id = data.get('operatorId', 'admin')
        
        result = api.acknowledge_alert_event(event_id, operator_id)
        return jsonify(result)
    except Exception as e:
        return jsonify({"code": -1, "message": str(e)}), 500

@app.route('/api/resolve-event', methods=['POST'])
def resolve_event():
    """解决告警事件"""
    try:
        data = request.get_json()
        event_id = data.get('eventId')
        
        result = api.resolve_alert_event(event_id)
        return jsonify(result)
    except Exception as e:
        return jsonify({"code": -1, "message": str(e)}), 500

@app.route('/api/create-alert-rule', methods=['POST'])
def create_alert_rule():
    """创建告警规则"""
    try:
        data = request.get_json()
        result = api.create_alert_rule(data)
        return jsonify(result)
    except Exception as e:
        return jsonify({"code": -1, "message": str(e)}), 500

@app.route('/api/update-alert-rule/<rule_id>', methods=['PUT'])
def update_alert_rule(rule_id):
    """更新告警规则"""
    try:
        data = request.get_json()
        result = api.update_alert_rule(rule_id, data)
        return jsonify(result)
    except Exception as e:
        return jsonify({"code": -1, "message": str(e)}), 500

@app.route('/api/delete-alert-rule/<rule_id>', methods=['DELETE'])
def delete_alert_rule(rule_id):
    """删除告警规则"""
    try:
        result = api.delete_alert_rule(rule_id)
        return jsonify(result)
    except Exception as e:
        return jsonify({"code": -1, "message": str(e)}), 500

@app.route('/api/get-alert-rule/<rule_id>')
def get_alert_rule(rule_id):
    """获取单个告警规则"""
    try:
        result = api.get_alert_rule(rule_id)
        return jsonify(result)
    except Exception as e:
        return jsonify({"code": -1, "message": str(e)}), 500

if __name__ == '__main__':
    print("=" * 50)
    print("监控系统前端仪表板")
    print("=" * 50)
    print(f"服务器地址: {SERVER_BASE_URL}")
    print(f"前端地址: http://localhost:5001")
    print(f"自动刷新间隔: {REFRESH_INTERVAL}秒")
    print("=" * 50)
    
    app.run(debug=True, host='0.0.0.0', port=5001)
