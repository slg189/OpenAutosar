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
import glob
import os
import shutil
import subprocess
import sys
import zipfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
VENV = os.path.join(TOOLS, '.venv')
REQ = os.path.join(TOOLS, 'requirements.txt')
OFFLINE = os.path.join(TOOLS, 'Offline')
SPLIT_OFFLINE_DIR = os.path.join(TOOLS, 'Offline_Tools_Windows_x64')
SPLIT_OFFLINE_PREFIX = 'OpenAutosar_Offline_Tools_Windows_x64.zip.'

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


def bundled_path(tool):
    candidates = {
        'gcc': [os.path.join(OFFLINE, 'tools', 'windows-x86_64', 'w64devkit', 'bin', 'gcc.exe')],
        'g++': [os.path.join(OFFLINE, 'tools', 'windows-x86_64', 'w64devkit', 'bin', 'g++.exe')],
        'cmake': glob.glob(os.path.join(TOOLS, '.toolcache', 'cmake-*', 'bin', 'cmake.exe')),
        'ctest': glob.glob(os.path.join(TOOLS, '.toolcache', 'cmake-*', 'bin', 'ctest.exe')),
        'cppcheck': glob.glob(os.path.join(OFFLINE, 'tools', 'windows-x86_64', 'cppcheck', '**', 'cppcheck.exe'), recursive=True),
        'doxygen': glob.glob(os.path.join(OFFLINE, 'tools', 'windows-x86_64', 'doxygen', '**', 'doxygen.exe'), recursive=True),
    }.get(tool, [])
    for path in candidates:
        if os.path.exists(path):
            return path
    return None


def tool_which(tool):
    return shutil.which(tool) or bundled_path(tool)


def _ver(tool):
    try:
        exe = tool_which(tool) or tool
        out = subprocess.run([exe, '--version'], capture_output=True, text=True, timeout=8)
        return (out.stdout or out.stderr).splitlines()[0].strip()
    except Exception:
        return ''


def check_tools():
    print('== 必需外部工具 (host 构建/检查/单测) ==')
    missing = []
    for t, why in REQUIRED.items():
        p = tool_which(t)
        print(f'  [{"OK" if p else "!!"}] {t:20} {_ver(t) if p else "缺失"}   ({why})')
        if not p:
            missing.append(t)
    print('== 可选工具 (远程/目标/文档) ==')
    for t, why in OPTIONAL.items():
        p = tool_which(t)
        print(f'  [{"OK" if p else "--"}] {t:20} {_ver(t) if p else "未装"}   ({why})')
    return missing


def venv_bin(name):
    sub = 'Scripts' if os.name == 'nt' else 'bin'
    exe = name + ('.exe' if os.name == 'nt' else '')
    return os.path.join(VENV, sub, exe)


def default_wheelhouse():
    """Return the best matching offline wheelhouse for the running Python."""
    tag = f'cp{sys.version_info.major}{sys.version_info.minor}'
    candidates = []
    for d in glob.glob(os.path.join(OFFLINE, 'wheelhouse', '*')):
        base = os.path.basename(d).lower()
        score = 0
        if tag in base:
            score += 10
        if sys.platform.startswith('win') and 'win' in base:
            score += 3
        elif sys.platform.startswith('linux') and 'linux' in base:
            score += 3
        elif sys.platform == 'darwin' and ('mac' in base or 'darwin' in base):
            score += 3
        if os.path.isdir(d) and glob.glob(os.path.join(d, '*.whl')):
            candidates.append((score, d))
    if not candidates:
        return None
    exact = [item for item in candidates if tag in os.path.basename(item[1]).lower()]
    if exact:
        return sorted(exact, reverse=True)[0][1]
    return None


def offline_payload_ready():
    return bool(default_wheelhouse()) and bool(bundled_path('cmake')) and bool(bundled_path('gcc'))


def offline_split_parts():
    if not os.path.isdir(SPLIT_OFFLINE_DIR):
        return []
    parts = []
    for name in sorted(os.listdir(SPLIT_OFFLINE_DIR)):
        if name.startswith(SPLIT_OFFLINE_PREFIX) and name[-3:].isdigit():
            parts.append(os.path.join(SPLIT_OFFLINE_DIR, name))
    return parts


def ensure_offline_payload_from_parts():
    """Rehydrate Tools/Offline and Tools/.toolcache from 10 split zip parts."""
    if offline_payload_ready():
        return
    parts = offline_split_parts()
    if len(parts) != 10:
        return
    joined = os.path.join(SPLIT_OFFLINE_DIR, '_joined_OpenAutosar_Offline_Tools_Windows_x64.zip')
    print(f'== 从分卷还原离线工具包: {os.path.relpath(SPLIT_OFFLINE_DIR, ROOT)} ==')
    try:
        with open(joined, 'wb') as out:
            for part in parts:
                with open(part, 'rb') as f:
                    shutil.copyfileobj(f, out, length=1024 * 1024)
        with zipfile.ZipFile(joined) as z:
            z.extractall(TOOLS)
    finally:
        if os.path.exists(joined):
            os.remove(joined)


def pip_install(vpy, args, offline=False, wheelhouse=None):
    cmd = [vpy, '-m', 'pip', 'install', '-q']
    if offline:
        if not wheelhouse or not os.path.isdir(wheelhouse):
            raise SystemExit(f'[fatal] offline wheelhouse not found: {wheelhouse or "(auto)"}')
        cmd += ['--no-index', '--find-links', wheelhouse]
    subprocess.check_call(cmd + args)


def write_offline_launcher():
    out = os.path.join(TOOLS, 'activate_offline.bat')
    lines = [
        '@echo off',
        'set "ROOT=%~dp0.."',
        'set "TOOLS=%ROOT%\\Tools"',
        'if exist "%TOOLS%\\Offline\\tools\\windows-x86_64\\w64devkit\\bin\\gcc.exe" set "PATH=%TOOLS%\\Offline\\tools\\windows-x86_64\\w64devkit\\bin;%PATH%"',
        'if exist "%TOOLS%\\.toolcache\\cmake-3.28.6-windows-x86_64\\bin\\cmake.exe" set "PATH=%TOOLS%\\.toolcache\\cmake-3.28.6-windows-x86_64\\bin;%PATH%"',
        'for /r "%TOOLS%\\Offline\\tools\\windows-x86_64\\cppcheck" %%I in (cppcheck.exe) do set "PATH=%%~dpI;%PATH%"',
        'for /r "%TOOLS%\\Offline\\tools\\windows-x86_64\\doxygen" %%I in (doxygen.exe) do set "PATH=%%~dpI;%PATH%"',
        'call "%TOOLS%\\.venv\\Scripts\\activate.bat"',
        'echo Offline OpenAutosar environment activated.',
    ]
    with open(out, 'w', encoding='utf-8', newline='\r\n') as f:
        f.write('\n'.join(lines) + '\n')
    return out


def _cmake_in_cache():
    import glob
    g = (glob.glob(os.path.join(TOOLS, '.toolcache', 'cmake-*', 'bin', 'cmake'))
         + glob.glob(os.path.join(TOOLS, '.toolcache', 'cmake-*', 'bin', 'cmake.exe'))
         + glob.glob(os.path.join(TOOLS, '.toolcache', 'cmake-*', 'CMake.app', 'Contents', 'bin', 'cmake')))
    return g[0] if g else None


def check_cmake_min(major=3, minor=16):
    """硬校验 cmake >= 3.16 (系统或 .toolcache)。返回 True/False。"""
    exe = tool_which('cmake') or _cmake_in_cache()
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
    ap.add_argument('--offline', action='store_true', help='从 Tools/Offline/wheelhouse 离线安装 Python 依赖')
    ap.add_argument('--wheelhouse', default=None, help='离线 wheelhouse 目录; 默认自动匹配 Tools/Offline/wheelhouse/*')
    a = ap.parse_args()

    if a.offline:
        ensure_offline_payload_from_parts()

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
    wheelhouse = a.wheelhouse or default_wheelhouse()
    if a.offline:
        print(f'== 离线 wheelhouse: {wheelhouse or "(not found)"} ==')
        if not wheelhouse:
            raise SystemExit(
                f'[fatal] no offline wheels for Python {sys.version_info.major}.{sys.version_info.minor}; '
                'install the packaged Python 3.11 or pass --wheelhouse explicitly'
            )
    pip_install(vpy, ['--upgrade', 'pip'], offline=a.offline, wheelhouse=wheelhouse)
    pip_install(vpy, ['-r', REQ], offline=a.offline, wheelhouse=wheelhouse)

    print('\n[done] 环境就绪。用法:')
    if os.name == 'nt':
        launcher = write_offline_launcher()
        print(r'  Tools\activate_offline.bat   &&  scons')
        print(f'  (launcher: {os.path.relpath(launcher, ROOT)})')
    else:
        print('  source Tools/.venv/bin/activate  &&  scons')
        print('  或直接:  Tools/.venv/bin/scons')
    print('  (run_gtest.py 会自动用 .toolcache 里的 cmake/googletest)')
    if missing:
        print(f'\n[warn] 缺少外部工具: {", ".join(missing)} (相应功能不可用; 见 Tools/ENVIRONMENT.md)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
