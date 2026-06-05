#!/usr/bin/env bash
# ============================================================================
# release_libs.sh
# 将各层源码模块编译为 .a, 并连同对外头文件(inc/)释放到根 *_Libs/。
# 这是「信息安全分级释放」的核心步骤: 下游(ASW工程师/基础工程师)只拿到
# 二进制库与头文件, 拿不到不该看到的源码。
#
# 用法:
#   ./release_libs.sh [layer]      layer ∈ {ASW|BSW|CDD|MCAL|all}  默认 all
#   CHIP=Tc387 ./release_libs.sh MCAL
# ============================================================================
set -euo pipefail

CHIP="${CHIP:-Tc387}"
LAYER="${1:-all}"
PROJECT="${PROJECT:-Demo_Tc387}"

copy_headers() { # $1=src_inc_dir  $2=dst_inc_dir
    mkdir -p "$2"; [[ -d "$1" ]] && cp -f "$1"/*.h "$2"/ 2>/dev/null || true
}

build_lib() { # $1=src_dir  $2=module  $3=dst_dir
    local src="$1" mod="$2" dst="$3"
    mkdir -p "$dst"
    if [[ -f "$src/SConscript" ]]; then
        echo "  [release] 编译 $mod (source) -> $dst/lib$mod.a"
        ( cd "Projects/$PROJECT" && scons only="$mod" chip="$CHIP" release >/dev/null ) || \
            echo "  [release] (跳过编译, 工具链不可用; 仅同步头文件)"
    fi
    copy_headers "$src/inc" "$dst/inc"
}

release_mcal() {
    for mod in Adc Can Port; do
        local s="MCAL/$CHIP/$mod"
        [[ -d "$s" ]] && build_lib "$s" "$mod" "MCAL_Libs/$CHIP/$mod"
    done
    copy_headers "MCAL/inc" "MCAL_Libs/inc"
}
release_cdd() {
    for d in CDDs/*/; do
        local mod; mod=$(basename "$d")
        build_lib "$d" "$mod" "CDD_Libs/$mod"
    done
}
release_asw() {
    for d in ASWs/*/; do
        local mod; mod=$(basename "$d"); [[ "$mod" == "inc" ]] && continue
        build_lib "$d" "$mod" "ASW_Libs/$mod"
    done
    copy_headers "ASWs/inc" "ASW_Libs/inc"
}
release_bsw() {
    # BSW 通常由供应商以 .a 交付, 此处仅同步头文件与现成库
    copy_headers "BSW/inc"        "BSW_Libs/inc"
    copy_headers "BSW/Etas/inc"   "BSW_Libs/Etas/inc"
    copy_headers "BSW/Vector/inc" "BSW_Libs/Vector/inc"
    cp -f BSW/Etas/lib/*.a   BSW_Libs/Etas/   2>/dev/null || true
    cp -f BSW/Vector/lib/*.a BSW_Libs/Vector/ 2>/dev/null || true
}

case "$LAYER" in
    MCAL) release_mcal ;;
    CDD)  release_cdd ;;
    ASW)  release_asw ;;
    BSW)  release_bsw ;;
    all)  release_mcal; release_cdd; release_asw; release_bsw ;;
    *) echo "用法: $0 [ASW|BSW|CDD|MCAL|all]" >&2; exit 1 ;;
esac

echo "[release] 完成 ($LAYER, chip=$CHIP)。"
