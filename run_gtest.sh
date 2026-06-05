#!/usr/bin/env bash
# ============================================================================
# run_gtest.sh
# 构建并运行 GoogleTest 单元测试 (Test/ut), 生成覆盖率到 coverage_output/。
# 用法: ./run_gtest.sh [module]   module 为空则运行全部
# ============================================================================
set -euo pipefail

BUILD_DIR="build_test"
COVERAGE_DIR="coverage_output"
REPORT_DIR="reports"
MODULE="${1:-all}"

mkdir -p "$BUILD_DIR" "$COVERAGE_DIR" "$REPORT_DIR"

echo "[gtest] 配置构建 (module=$MODULE) ..."
cmake -S Test/ut -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE=Coverage \
      -DGTEST_MODULE="$MODULE"

echo "[gtest] 编译 ..."
cmake --build "$BUILD_DIR" -j"$(nproc)"

echo "[gtest] 运行测试 ..."
ctest --test-dir "$BUILD_DIR" --output-on-failure \
      --output-junit "../$REPORT_DIR/gtest_results.xml"

echo "[gtest] 采集覆盖率 ..."
lcov --capture --directory "$BUILD_DIR" --output-file "$COVERAGE_DIR/coverage.info" 2>/dev/null || true
genhtml "$COVERAGE_DIR/coverage.info" --output-directory "$COVERAGE_DIR/html" 2>/dev/null || true

echo "[gtest] 完成。报告: $REPORT_DIR/gtest_results.xml  覆盖率: $COVERAGE_DIR/html/index.html"
