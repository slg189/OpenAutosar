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


def check_tools():
    print('== 必需外部工具 (host 构建/检查/单测) ==')
    missing = []
    for t, why in REQUIRED.items():
        p = shutil.which(t)
        print(f'  [{"OK" if p else "!!"}] {t:20} {_ver(t) if p else "缺失"}   ({why})')
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


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--check', action='store_true', help='只检查外部工具, 不建 venv')
    ap.add_argument('--force', action='store_true', help='重建已存在的 venv')
    a = ap.parse_args()

    missing = check_tools()
    if a.check:
        return 1 if missing else 0

    if os.path.isdir(VENV) and a.force:
        shutil.rmtree(VENV)
    if not os.path.isdir(VENV):
        print(f'\n== 创建 venv: {os.path.relpath(VENV, ROOT)} ==')
        subprocess.check_call([sys.executable, '-m', 'venv', VENV])
    print('== 安装 pinned 依赖 (Tools/requirements.txt) ==')
    vpy = venv_bin('python')
    subprocess.check_call([vpy, '-m', 'pip', 'install', '-q', '--upgrade', 'pip'])
    subprocess.check_call([vpy, '-m', 'pip', 'install', '-q', '-r', REQ])

    print('\n[done] 隔离环境就绪。激活后用 scons:')
    if os.name == 'nt':
        print(r'  Tools\.venv\Scripts\activate   &&  scons')
    else:
        print('  source Tools/.venv/bin/activate  &&  scons')
        print('  或直接:  Tools/.venv/bin/scons')
    if missing:
        print(f'\n[warn] 缺少外部工具: {", ".join(missing)} (相应功能不可用; 见 Tools/ENVIRONMENT.md)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
