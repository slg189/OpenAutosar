#!/usr/bin/env bash
# ============================================================================
# run_integrationTest.sh
# 在 QEMU 上运行集成测试 (依据 TestPlatform_UintTest_QEMU_Test.json 配置)。
# 用法: ./run_integrationTest.sh [project]   默认 Demo_Tc387
# ============================================================================
set -euo pipefail

PROJECT="${1:-Demo_Tc387}"
CONFIG="TestPlatform_UintTest_QEMU_Test.json"
REPORT_DIR="reports"

mkdir -p "$REPORT_DIR"

echo "[itest] 构建工程 $PROJECT ..."
( cd "Projects/$PROJECT" && scons -j"$(nproc)" )

ELF="Projects/$PROJECT/Out/$PROJECT.elf"
if [[ ! -f "$ELF" ]]; then
    echo "[itest] 未找到 ELF: $ELF" >&2
    exit 1
fi

echo "[itest] 在 QEMU 中运行集成测试 (config=$CONFIG) ..."
python3 Test/Projects/run_qemu.py \
        --config "$CONFIG" \
        --elf "$ELF" \
        --report "$REPORT_DIR/integration_${PROJECT}.xml"

echo "[itest] 完成。报告: $REPORT_DIR/integration_${PROJECT}.xml"
