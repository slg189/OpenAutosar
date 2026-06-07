#!/usr/bin/env python3
"""一键搭建构建所需 Python 环境 (仓库内隔离 venv) + 校验外部工具。

目的: 把构建用的 Python 依赖固定版本、装进仓库内的 Tools/.venv,
      使不同电脑的系统 Python / SCons 版本差异不影响构建 ("避免环境影响")。

用法:
    python Tools/setup_env.py            # 创建 Tools/.venv 并装 pinned 依赖 (Tools/requirements.txt)
    python Tools/setup_env.py --check    # 只检查外部工具 (cmake/cppcheck/gcc...) 是否齐备
    python Tools/setup_env.py --force    # 重建 venv

搭好后:
    Linux/mac:  source Tools/.venv/bin/activate   然后  scons
    Windows  :  Tools\\.venv\\Scripts\\activate     然后  scons
    或直接   :  Tools/.venv/bin/scons   (Windows: Tools\\.venv\\Scripts\\scons.exe)
"""
import argparse
import os
import shutil
import subprocess
import sys

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
VENV = os.path.join(TOOLS, '.venv')
REQ = os.path.join(TOOLS, 'requirements.txt')
VENDOR_WHEELS = os.path.join(TOOLS, 'vendor', 'wheels')   # 离线 pip (随仓库提交的 wheel)

# 外部(系统)工具: 名称 -> 用途。缺失只提示, 不强制 (按场景而定)。
REQUIRED = {
    'git':      '版本控制 + GoogleTest FetchContent 拉取',
    'gcc':      'host 验证编译 (TOOLCHAIN=gcc)',
    'g++':      'GoogleTest 单元测试编译',
    'cmake':    '单元测试构建 (>=3.16)',
    'ctest':    '单元测试运行',
    'cppcheck': '静态检查 / MISRA (scons check)',
}
OPTIONAL = {
    'doxygen':             'API 文档 (scons doc, 可选)',
    'rsync':               '远程构建文件同步 (scons --remote)',
    'ssh':                 '远程构建 (scons --remote)',
    'tricore-gcc':         '目标(AURIX)编译 (hightec 工具链)',
    'qemu-system-tricore': '集成测试 (QEMU, 第二道闸)',
}


def _ver(tool):
    try:
        out = subprocess.run([tool, '--version'], capture_output=True, text=True, timeout=8)
        return (out.stdout or out.stderr).splitlines()[0].strip()
    except Exception:
        return ''


def _cppcheck_in_cache():
    import glob
    g = glob.glob(os.path.join(TOOLS, '.toolcache', 'cppcheck', '**', 'cppcheck.exe'), recursive=True)
    return g[0] if g else None


def _resolve(tool):
    """系统 PATH 优先; cmake/ctest/cppcheck 再回退到 Tools/.toolcache (vendor 解压而来)。"""
    p = shutil.which(tool)
    if p:
        return p
    if tool in ('cmake', 'ctest'):
        c = _cmake_in_cache()
        if not c:
            return None
        if tool == 'cmake':
            return c
        # 同目录的 ctest (只换 basename, 不动含 'cmake' 的目录名)
        return os.path.join(os.path.dirname(c), 'ctest' + ('.exe' if c.endswith('.exe') else ''))
    if tool == 'cppcheck':
        return _cppcheck_in_cache()
    return None


def check_tools():
    print('== 必需外部工具 (host 构建/检查/单测) ==')
    missing = []
    for t, why in REQUIRED.items():
        p = _resolve(t)
        src = '' if (p and shutil.which(t)) else (' [.toolcache]' if p else '')
        print(f'  [{"OK" if p else "!!"}] {t:20} {(_ver(p) if p else "缺失")}{src}   ({why})')
        if not p:
            missing.append(t)
    print('== 可选工具 (远程/目标/文档) ==')
    for t, why in OPTIONAL.items():
        p = shutil.which(t)
        print(f'  [{"OK" if p else "--"}] {t:20} {_ver(t) if p else "未装"}   ({why})')
    return missing


def venv_bin(name):
    sub = 'Scripts' if os.name == 'nt' else 'bin'
    exe = name + ('.exe' if os.name == 'nt' else '')
    return os.path.join(VENV, sub, exe)


def _cmake_in_cache():
    import glob
    tc = os.path.join(TOOLS, '.toolcache')
    g = (glob.glob(os.path.join(tc, 'cmake-*', 'bin', 'cmake'))
         + glob.glob(os.path.join(tc, 'cmake-*', 'bin', 'cmake.exe'))          # Windows
         + glob.glob(os.path.join(tc, 'cmake-*', 'CMake.app', 'Contents', 'bin', 'cmake')))
    return g[0] if g else None


def _vendored_wheels():
    """Tools/vendor/wheels 下有 wheel 时返回该目录 (离线 pip), 否则 None。"""
    import glob
    return VENDOR_WHEELS if glob.glob(os.path.join(VENDOR_WHEELS, '*.whl')) else None


def check_cmake_min(major=3, minor=16):
    """硬校验 cmake >= 3.16 (系统或 .toolcache)。返回 True/False。"""
    exe = shutil.which('cmake') or _cmake_in_cache()
    if not exe:
        print(f'  [!!] cmake             未找到 (需 >= {major}.{minor}; 可由本脚本下载到 .toolcache)')
        return False
    out = _ver(exe.replace('cmake', 'cmake') if os.path.isabs(exe) else exe)
    try:
        nums = [int(x) for x in out.split('version')[-1].strip().split('.')[:2]]
        ok = (nums[0], nums[1]) >= (major, minor)
    except Exception:
        ok = True
    print(f'  [{"OK" if ok else "!!"}] cmake 版本          {out}  (需 >= {major}.{minor})')
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--check', action='store_true', help='只检查外部工具/版本, 不建 venv/下载')
    ap.add_argument('--force', action='store_true', help='重建已存在的 venv')
    ap.add_argument('--no-fetch', action='store_true', help='不下载固定工具到 .toolcache (cmake/googletest)')
    a = ap.parse_args()

    missing = check_tools()
    print('== 版本硬校验 ==')
    cmake_ok = check_cmake_min(3, 16)
    if a.check:
        return 0 if (not missing and cmake_ok) else 1

    # 1) 下载固定工具 (cmake/googletest) 到 Tools/.toolcache
    if not a.no_fetch:
        print('\n== 下载固定工具到 Tools/.toolcache (fetch_tools.py) ==')
        subprocess.call([sys.executable, os.path.join(TOOLS, 'fetch_tools.py')])

    # 2) 建隔离 venv + pinned Python 依赖
    if os.path.isdir(VENV) and a.force:
        shutil.rmtree(VENV)
    if not os.path.isdir(VENV):
        print(f'\n== 创建 venv: {os.path.relpath(VENV, ROOT)} ==')
        subprocess.check_call([sys.executable, '-m', 'venv', VENV])
    print('== 安装 pinned 依赖 (Tools/requirements.txt) ==')
    vpy = venv_bin('python')
    # pip 自升级是 best-effort: 离线机器升不了 pip 不应中断 (venv 自带的 pip 足够用)
    try:
        subprocess.check_call([vpy, '-m', 'pip', 'install', '-q', '--upgrade', 'pip'])
    except subprocess.CalledProcessError:
        print('  [warn] 跳过 pip 自升级 (可能离线), 用 venv 自带 pip')
    wheels = _vendored_wheels()
    if wheels:
        print(f'  [offline] 用仓库内 wheel 安装 (不联网): {os.path.relpath(wheels, ROOT)}')
        subprocess.check_call([vpy, '-m', 'pip', 'install', '-q',
                               '--no-index', '--find-links', wheels, '-r', REQ])
    else:
        subprocess.check_call([vpy, '-m', 'pip', 'install', '-q', '-r', REQ])

    print('\n[done] 环境就绪。用法:')
    if os.name == 'nt':
        print(r'  Tools\.venv\Scripts\activate   &&  scons')
    else:
        print('  source Tools/.venv/bin/activate  &&  scons')
        print('  或直接:  Tools/.venv/bin/scons')
    print('  (run_gtest.py 会自动用 .toolcache 里的 cmake/googletest)')
    if missing:
        print(f'\n[warn] 缺少外部工具: {", ".join(missing)} (相应功能不可用; 见 Tools/ENVIRONMENT.md)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
