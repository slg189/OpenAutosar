#!/usr/bin/env bash
# =============================================================
# OpenAutosar SCons v2 一键部署脚本（Bash / Git Bash / WSL）
#
# 用法：
#   ./deploy.sh                    # 部署全部 + 演示模块
#   ./deploy.sh --no-demo          # 不部署演示模块
#   ./deploy.sh --dry-run          # 只显示
#   ./deploy.sh --force            # 覆盖已存在
# =============================================================
set -euo pipefail

NO_DEMO=0
DRY_RUN=0
FORCE=0
for arg in "$@"; do
    case "$arg" in
        --no-demo)  NO_DEMO=1 ;;
        --dry-run)  DRY_RUN=1 ;;
        --force)    FORCE=1 ;;
        *) echo "未知参数: $arg"; exit 1 ;;
    esac
done

PROTO="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$PROTO/../.." && pwd)"

echo "源目录: $PROTO"
echo "目标根: $ROOT"
echo

copy_logged() {
    local src="$1" dst="$2"
    if [[ ! -e "$src" ]]; then
        echo "  [WARN] 源不存在：$src（跳过）" >&2
        return
    fi
    echo "  $src"
    echo "   → $dst"
    [[ "$DRY_RUN" == "1" ]] && return
    mkdir -p "$(dirname "$dst")"
    local opts="-r"
    [[ "$FORCE" == "1" ]] && opts="-rf"
    cp $opts "$src" "$dst"
}

echo "① 部署顶层 SConstruct"
copy_logged "$PROTO/SConstruct" "$ROOT/SConstruct"

echo; echo "② 部署 site_scons (Python 包)"
copy_logged "$PROTO/site_scons" "$ROOT/site_scons"

echo; echo "③ 部署 scripts"
copy_logged "$PROTO/scripts" "$ROOT/scripts"

echo; echo "④ 部署项目配置"
PROJ="$ROOT/Projects/DemoProject"
mkdir -p "$PROJ"
copy_logged "$PROTO/projects/DemoProject/build.yaml"      "$PROJ/build.yaml"
copy_logged "$PROTO/projects/DemoProject/build.lite.yaml" "$PROJ/build.lite.yaml"
copy_logged "$PROTO/projects/DemoProject/build.qemu.yaml" "$PROJ/build.qemu.yaml"
copy_logged "$PROTO/projects/DemoProject/SConscript"      "$PROJ/SConscript"

echo; echo "⑤ 部署 Doxygen 模板"
copy_logged "$PROTO/doxygen/Doxyfile.in" "$ROOT/docs/Doxyfile.in"

if [[ "$NO_DEMO" == "0" ]]; then
    echo; echo "⑥ 部署演示模块 BSW/Crc"
    copy_logged "$PROTO/demo_bsw/Crc" "$ROOT/BSW/Crc"
else
    echo; echo "⑥ 跳过演示模块（--no-demo）"
fi

echo
if [[ "$DRY_RUN" == "1" ]]; then
    echo "[DRY-RUN] 未实际复制任何文件"
else
    echo "✓ 部署完成"
    echo
    echo "下一步："
    echo "  1) pip install -r docs/v2/requirements.txt"
    echo "  2) cd $ROOT"
    echo "  3) scons -j8                # 冒烟构建"
    echo "  4) scons doc                # 文档生成"
    echo "  5) scons -c                 # 清理"
fi
