#!/usr/bin/env python3
"""把【可再分发】的构建工具下载并固定到仓库内 Tools/.toolcache/ (gitignore),
使不同电脑用到完全一致的工具版本, 且可离线复用。

可下载 (开源/可再分发):
  - CMake       官方 portable 二进制 (固定版本)  -> 单元测试构建
  - GoogleTest  源码 (固定 tag)                  -> 离线单测 (GOOGLETEST_DIR)
不下载 (本脚本只提示, 见 Tools/ENVIRONMENT.md):
  - gcc/g++              体积大, 用系统
  - cppcheck            Linux 无官方 portable, 用系统(apt)/源码构建
  - TriCore (hightec/tasking)  厂商授权, 不可再分发 -> 由你安装并加入 PATH
  - qemu-system-tricore 按需自建

用法:
  python Tools/fetch_tools.py             # 下载缺失工具到 .toolcache
  python Tools/fetch_tools.py --force     # 重新下载
  python Tools/fetch_tools.py --print-env # 打印需要 export 的 PATH / GOOGLETEST_DIR
"""
import argparse
import glob
import os
import platform
import sys
import tarfile
import urllib.request
import zipfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
CACHE = os.path.join(TOOLS, '.toolcache')

# —— 固定版本 (升级改这里并重测) ——
CMAKE_VERSION = '3.28.6'
GTEST_TAG = 'v1.14.0'


def _plat():
    s, m = platform.system(), platform.machine().lower()
    if s == 'Linux':
        return f'linux-{"aarch64" if m in ("aarch64", "arm64") else "x86_64"}', 'tar.gz'
    if s == 'Darwin':
        return 'macos-universal', 'tar.gz'
    if s == 'Windows':
        return 'windows-x86_64', 'zip'
    raise SystemExit(f'不支持的平台: {s}/{m}')


def cmake_dir():
    p, _ = _plat()
    return os.path.join(CACHE, f'cmake-{CMAKE_VERSION}-{p}')


def cmake_bin():
    if platform.system() == 'Darwin':
        return os.path.join(cmake_dir(), 'CMake.app', 'Contents', 'bin')
    return os.path.join(cmake_dir(), 'bin')


def gtest_dir():
    return os.path.join(CACHE, f'googletest-{GTEST_TAG.lstrip("v")}')


def _download(url, dest):
    print(f'[fetch] {url}')
    with urllib.request.urlopen(url, timeout=120) as r, open(dest, 'wb') as f:
        while True:
            chunk = r.read(1 << 20)
            if not chunk:
                break
            f.write(chunk)


def _extract(archive, dest):
    if archive.endswith('.zip'):
        with zipfile.ZipFile(archive) as z:
            z.extractall(dest)
    else:
        with tarfile.open(archive) as t:
            t.extractall(dest)   # 来源为 Kitware/Google 官方, 可信


def fetch_cmake(force):
    if os.path.isdir(cmake_dir()) and not force:
        print(f'[skip] cmake 已存在: {os.path.relpath(cmake_dir(), TOOLS)}')
        return
    p, ext = _plat()
    url = (f'https://github.com/Kitware/CMake/releases/download/'
           f'v{CMAKE_VERSION}/cmake-{CMAKE_VERSION}-{p}.{ext}')
    arc = os.path.join(CACHE, f'_cmake.{ext}')
    _download(url, arc)
    _extract(arc, CACHE)
    os.remove(arc)
    print(f'[ok] cmake {CMAKE_VERSION} -> {os.path.relpath(cmake_bin(), TOOLS)}')


def fetch_gtest(force):
    if os.path.isdir(gtest_dir()) and not force:
        print(f'[skip] googletest 已存在: {os.path.relpath(gtest_dir(), TOOLS)}')
        return
    url = f'https://github.com/google/googletest/archive/refs/tags/{GTEST_TAG}.tar.gz'
    arc = os.path.join(CACHE, '_gtest.tar.gz')
    _download(url, arc)
    _extract(arc, CACHE)
    os.remove(arc)
    print(f'[ok] googletest {GTEST_TAG} -> {os.path.relpath(gtest_dir(), TOOLS)}')


def print_env():
    win = os.name == 'nt'
    sep = ';' if win else ':'
    print('\n# 把以下加入环境 (setup_env.py 激活脚本会自动带上):')
    if os.path.isdir(cmake_bin()):
        print(f'set PATH={cmake_bin()};%PATH%' if win else f'export PATH="{cmake_bin()}{sep}$PATH"')
    if os.path.isdir(gtest_dir()):
        print(f'set GOOGLETEST_DIR={gtest_dir()}' if win else f'export GOOGLETEST_DIR="{gtest_dir()}"')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--force', action='store_true', help='重新下载')
    ap.add_argument('--print-env', action='store_true', help='只打印 PATH/GOOGLETEST_DIR')
    a = ap.parse_args()
    os.makedirs(CACHE, exist_ok=True)
    if a.print_env:
        print_env()
        return 0
    fetch_cmake(a.force)
    fetch_gtest(a.force)
    print('\n[done] 已固定到 Tools/.toolcache/ (gitignore)。run_gtest.py 会自动优先使用其中的 cmake/googletest。')
    print_env()
    return 0


if __name__ == '__main__':
    sys.exit(main())
