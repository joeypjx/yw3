#!/bin/bash

# 项目根目录编译脚本
# 调用 scripts 目录中的统一编译脚本

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 调用 scripts 目录中的编译脚本
exec "$SCRIPT_DIR/scripts/build_all.sh" "$@"
