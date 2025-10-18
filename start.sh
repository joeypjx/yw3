#!/bin/bash

# 项目根目录启动脚本
# 调用 scripts 目录中的完整系统启动脚本

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 调用 scripts 目录中的启动脚本
exec "$SCRIPT_DIR/scripts/start_full_system.sh" "$@"
