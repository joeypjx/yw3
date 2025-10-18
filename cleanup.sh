#!/bin/bash

# 项目根目录清理脚本
# 调用 scripts 目录中的进程清理脚本

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 调用 scripts 目录中的清理脚本
exec "$SCRIPT_DIR/scripts/cleanup_processes.sh" "$@"
