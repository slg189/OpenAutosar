#!/usr/bin/env bash
# 设置 TriCore 交叉工具链环境变量
export TRICORE_CC="${TRICORE_CC:-tricore-gcc}"
export TRICORE_AR="${TRICORE_AR:-tricore-ar}"
export PATH="${TRICORE_TOOLCHAIN_BIN:-/opt/tricore/bin}:$PATH"
