#!/usr/bin/env python3
"""把【可再分发】的构建工具准备到仓库内 Tools/.toolcache/ (gitignore),
使不同电脑用到完全一致的工具版本。

**离线优先 (offline-first)**:
  如果 Tools/vendor/ 下已随仓库提交了对应归档 (见 Tools/vendor/manifest.json),
  本脚本直接从 vendor/ 复制+解压, **完全不联网**; 仅当 vendor/ 缺该平台归档时才回退下载。
  => 网络受限的机器, 克隆仓库后 `python Tools/setup_env.py` 即可, 无需任何外网下载。

处理的工具 (开源/可再分发):
  - CMake       官方 portable 二进制 (固定版本)  -> 单元测试构建
  - GoogleTest  源码 (固定 tag)                  -> 离线单测 (FetchContent 不联网)
  - cppcheck    官方 Windows MSI (msiexec /a 解压为便携版) -> scons check 静态/MISRA
不处理 (见 Tools/ENVIRONMENT.md):
  - gcc/g++ / MSVC      host 编译器, 体积大, 用系统
  - TriCore (hightec/tasking)  厂商授权, 不可再分发 -> 自行安装并加入 PATH
  - qemu-system-tricore 按需自建

用法:
  python Tools/fetch_tools.py             # 准备缺失工具到 .toolcache (优先用 vendor/)
  python Tools/fetch_tools.py --force     # 重新解压/下载
  python Tools/fetch_tools.py --print-env # 打印需要 export 的 PATH / GOOGLETEST_DIR
"""
import argparse
import glob
import os
import platform
import shutil
import subprocess
import sys
import tarfile
import urllib.request
import zipfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
CACHE = os.path.join(TOOLS, '.toolcache')
VENDOR = os.path.join(TOOLS, 'vendor')
VENDOR_ARCHIVES = os.path.join(VENDOR, 'archives')
VENDOR_INSTALLERS = os.path.join(VENDOR, 'installers')

# —— 固定版本 (升级改这里 + 换 Tools/vendor/ 下归档并重测) ——
CMAKE_VERSION = '3.28.6'
GTEST_TAG = 'v1.14.0'
CPPCHECK_VERSION = '2.13.0'


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


def cppcheck_dir():
    return os.path.join(CACHE, 'cppcheck')


def cppcheck_exe():
    """便携 cppcheck.exe 路径 (从 MSI 解压而来), 找不到返回 None。"""
    hits = glob.glob(os.path.join(cppcheck_dir(), '**', 'cppcheck.exe'), recursive=True)
    return hits[0] if hits else None


def _download(url, dest):
    print(f'[fetch] 下载 {url}')
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


def _obtain(vendor_path, url, work_archive):
    """离线优先: vendor_path 存在则复制它; 否则下载到 work_archive。返回可解压的归档路径。"""
    if os.path.isfile(vendor_path):
        print(f'[vendor] 使用仓库内归档 (离线): {os.path.relpath(vendor_path, TOOLS)}')
        shutil.copyfile(vendor_path, work_archive)
        return work_archive
    _download(url, work_archive)
    return work_archive


def fetch_cmake(force):
    if os.path.isdir(cmake_dir()) and not force:
        print(f'[skip] cmake 已就绪: {os.path.relpath(cmake_dir(), TOOLS)}')
        return
    p, ext = _plat()
    vendor = os.path.join(VENDOR_ARCHIVES, f'cmake-{CMAKE_VERSION}-{p}.{ext}')
    url = (f'https://github.com/Kitware/CMake/releases/download/'
           f'v{CMAKE_VERSION}/cmake-{CMAKE_VERSION}-{p}.{ext}')
    arc = os.path.join(CACHE, f'_cmake.{ext}')
    _obtain(vendor, url, arc)
    _extract(arc, CACHE)
    os.remove(arc)
    print(f'[ok] cmake {CMAKE_VERSION} -> {os.path.relpath(cmake_bin(), TOOLS)}')


def fetch_gtest(force):
    if os.path.isdir(gtest_dir()) and not force:
        print(f'[skip] googletest 已就绪: {os.path.relpath(gtest_dir(), TOOLS)}')
        return
    vendor = os.path.join(VENDOR_ARCHIVES, f'googletest-{GTEST_TAG.lstrip("v")}.tar.gz')
    url = f'https://github.com/google/googletest/archive/refs/tags/{GTEST_TAG}.tar.gz'
    arc = os.path.join(CACHE, '_gtest.tar.gz')
    _obtain(vendor, url, arc)
    _extract(arc, CACHE)
    os.remove(arc)
    print(f'[ok] googletest {GTEST_TAG} -> {os.path.relpath(gtest_dir(), TOOLS)}')


def fetch_cppcheck(force):
    """仅 Windows: 把 vendor/ 里的 cppcheck MSI 用 msiexec /a 解压为便携版到 .toolcache/cppcheck。
    其它平台用系统 cppcheck (apt/brew), 此处跳过。"""
    if platform.system() != 'Windows':
        print('[skip] cppcheck: 非 Windows, 用系统 cppcheck (apt/brew install cppcheck)')
        return
    if cppcheck_exe() and not force:
        print(f'[skip] cppcheck 已就绪: {os.path.relpath(cppcheck_exe(), TOOLS)}')
        return
    msi = os.path.join(VENDOR_INSTALLERS, f'cppcheck-{CPPCHECK_VERSION}-x64-Setup.msi')
    if os.path.isfile(msi):
        print(f'[vendor] 使用仓库内 MSI (离线): {os.path.relpath(msi, TOOLS)}')
    else:
        url = (f'https://github.com/danmar/cppcheck/releases/download/'
               f'{CPPCHECK_VERSION}/cppcheck-{CPPCHECK_VERSION}-x64-Setup.msi')
        msi = os.path.join(CACHE, '_cppcheck.msi')
        _download(url, msi)
    target = cppcheck_dir()
    if force and os.path.isdir(target):
        shutil.rmtree(target, ignore_errors=True)
    os.makedirs(target, exist_ok=True)
    # msiexec /a = administrative install: 仅展开文件树, 不写注册表/不真正安装
    print('[cppcheck] msiexec /a 解压 MSI 为便携版 ...')
    rc = subprocess.call(['msiexec', '/a', msi, '/qb', f'TARGETDIR={os.path.abspath(target)}'])
    exe = cppcheck_exe()
    if rc != 0 or not exe:
        print(f'[warn] cppcheck MSI 解压失败 (rc={rc}); 可改用系统安装的 cppcheck', file=sys.stderr)
        return
    print(f'[ok] cppcheck {CPPCHECK_VERSION} -> {os.path.relpath(exe, TOOLS)}')


def print_env():
    win = os.name == 'nt'
    sep = ';' if win else ':'
    print('\n# 把以下加入环境 (setup_env.py 激活脚本会自动带上):')
    if os.path.isdir(cmake_bin()):
        print(f'set PATH={cmake_bin()};%PATH%' if win else f'export PATH="{cmake_bin()}{sep}$PATH"')
    if os.path.isdir(gtest_dir()):
        print(f'set GOOGLETEST_DIR={gtest_dir()}' if win else f'export GOOGLETEST_DIR="{gtest_dir()}"')
    exe = cppcheck_exe()
    if exe:
        d = os.path.dirname(exe)
        print(f'set PATH={d};%PATH%' if win else f'export PATH="{d}{sep}$PATH"')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--force', action='store_true', help='重新解压/下载')
    ap.add_argument('--print-env', action='store_true', help='只打印 PATH/GOOGLETEST_DIR')
    a = ap.parse_args()
    os.makedirs(CACHE, exist_ok=True)
    if a.print_env:
        print_env()
        return 0
    fetch_cmake(a.force)
    fetch_gtest(a.force)
    fetch_cppcheck(a.force)
    print('\n[done] 工具已就绪于 Tools/.toolcache/ (gitignore)。'
          'run_gtest.py / run_check.py 会自动优先使用其中的 cmake/googletest/cppcheck。')
    print_env()
    return 0


if __name__ == '__main__':
    sys.exit(main())
