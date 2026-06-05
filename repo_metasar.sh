#!/usr/bin/env bash
# ============================================================================
# repo_metasar.sh
# 通过 google-repo (manifest) 同步 OpenAutosar 各子仓库 (BSW / MCAL / CDD 等)。
# 用法: ./repo_metasar.sh [init|sync|status]
# ============================================================================
set -euo pipefail

MANIFEST_URL="${MANIFEST_URL:-ssh://git@example.com/autosar/manifest.git}"
MANIFEST_BRANCH="${MANIFEST_BRANCH:-main}"
REPO_DIR="Repo"

cmd="${1:-sync}"

case "$cmd" in
    init)
        echo "[metasar] 初始化 repo manifest: $MANIFEST_URL ($MANIFEST_BRANCH)"
        mkdir -p "$REPO_DIR"
        (cd "$REPO_DIR" && repo init -u "$MANIFEST_URL" -b "$MANIFEST_BRANCH")
        ;;
    sync)
        echo "[metasar] 同步所有子仓库 ..."
        (cd "$REPO_DIR" && repo sync -j"$(nproc)")
        ;;
    status)
        (cd "$REPO_DIR" && repo status)
        ;;
    *)
        echo "用法: $0 [init|sync|status]" >&2
        exit 1
        ;;
esac
