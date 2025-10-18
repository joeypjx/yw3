# 双标签方式使用示例

## 📋 **标签优先级规则**

1. **条件级标签优先**：如果条件有自己的 `tags`，优先使用条件的 `tags`
2. **表达式级标签备用**：如果条件没有 `tags`，使用表达式的全局 `tags`
3. **无标签过滤**：如果都没有 `tags`，不进行标签过滤

## 🎯 **支持的JSON格式**

### **1. 条件级标签（原有方式）**
```json
{
  "alert_name": "条件级标签告警",
  "expression": {
    "conditions": [
      {
        "metric": "cpu.usage_percent",
        "operator": ">",
        "threshold": 80,
        "duration": "2m",
        "tags": {
          "node_id": "server1",
          "env": "production"
        }
      }
    ],
    "logic": "AND"
  }
}
```

### **2. 表达式级标签（新方式）**
```json
{
  "alert_name": "表达式级标签告警",
  "expression": {
    "conditions": [
      {
        "metric": "cpu.usage_percent",
        "operator": ">",
        "threshold": 80,
        "duration": "2m"
      },
      {
        "metric": "memory.usage_percent",
        "operator": ">",
        "threshold": 90,
        "duration": "1m"
      }
    ],
    "logic": "AND",
    "tags": {
      "node_id": "server1",
      "env": "production",
      "region": "us-west-1"
    }
  }
}
```

### **3. 混合标签（条件级优先）**
```json
{
  "alert_name": "混合标签告警",
  "expression": {
    "conditions": [
      {
        "metric": "cpu.usage_percent",
        "operator": ">",
        "threshold": 80,
        "duration": "2m",
        "tags": {
          "node_id": "server1",
          "env": "production"
        }
      },
      {
        "metric": "memory.usage_percent",
        "operator": ">",
        "threshold": 90,
        "duration": "1m"
      }
    ],
    "logic": "AND",
    "tags": {
      "node_id": "server2",
      "env": "staging",
      "region": "us-east-1"
    }
  }
}
```

**说明**：
- 第一个条件使用自己的标签：`node_id: server1, env: production`
- 第二个条件使用全局标签：`node_id: server2, env: staging, region: us-east-1`

### **4. 无标签**
```json
{
  "alert_name": "无标签告警",
  "expression": {
    "conditions": [
      {
        "metric": "cpu.usage_percent",
        "operator": ">",
        "threshold": 80,
        "duration": "2m"
      }
    ],
    "logic": "AND"
  }
}
```

## 🔧 **技术实现**

### **标签选择逻辑**
```cpp
// 确定使用哪个标签集合：条件级标签优先，否则使用全局标签
std::map<std::string, std::string> tagsToUse;
if (!condition.tags.empty()) {
    tagsToUse = condition.tags;  // 使用条件级标签
} else if (!globalTags.empty()) {
    tagsToUse = globalTags;     // 使用表达式级标签
}
```

### **JSON解析支持**
- **条件级标签**：`conditions[].tags`
- **表达式级标签**：`expression.tags`
- **向后兼容**：原有的条件级标签仍然有效

## 📊 **使用场景**

### **场景1：环境隔离**
```json
{
  "expression": {
    "conditions": [
      {"metric": "cpu.usage_percent", "operator": ">", "threshold": 80},
      {"metric": "memory.usage_percent", "operator": ">", "threshold": 90}
    ],
    "logic": "AND",
    "tags": {"env": "production"}
  }
}
```

### **场景2：特定节点监控**
```json
{
  "expression": {
    "conditions": [
      {
        "metric": "cpu.usage_percent",
        "operator": ">",
        "threshold": 80,
        "tags": {"node_id": "web-server-01"}
      },
      {
        "metric": "disk.usage_percent",
        "operator": ">",
        "threshold": 85,
        "tags": {"node_id": "db-server-01"}
      }
    ],
    "logic": "OR"
  }
}
```

### **场景3：混合环境告警**
```json
{
  "expression": {
    "conditions": [
      {
        "metric": "cpu.usage_percent",
        "operator": ">",
        "threshold": 80,
        "tags": {"env": "production"}
      },
      {
        "metric": "memory.usage_percent",
        "operator": ">",
        "threshold": 90
      }
    ],
    "logic": "AND",
    "tags": {"region": "us-west-1"}
  }
}
```

**说明**：
- 第一个条件：只对生产环境的节点生效
- 第二个条件：对us-west-1区域的所有节点生效

## ✅ **优势**

1. **向后兼容**：原有的条件级标签仍然有效
2. **灵活性**：支持两种标签方式，满足不同场景需求
3. **优先级清晰**：条件级标签优先，避免冲突
4. **配置简化**：表达式级标签减少重复配置
5. **功能强大**：支持复杂的标签组合场景
