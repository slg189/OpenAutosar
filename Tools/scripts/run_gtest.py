#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""构建并运行 GoogleTest 单元测试 (跨平台, 取代 run_gtest.bat)。

单元测试位于 Projects/<P>/Test/ut, 报告输出到 Projects/<P>/Reports。
用法:
    python Tools/scripts/run_gtest.py [--project Demo_Tc387] [--module all]
覆盖率: 若有 gcovr/OpenCppCoverage 则采集, 否则跳过。
"""
import argparse
import os
import shutil
import subprocess
import sys

import glob

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
TOOLCACHE = os.path.join(ROOT, 'Tools', '.toolcache')


def _cmake():
    """优先用系统 cmake; 否则用 Tools/.toolcache 里下载固定的 cmake。"""
    exe = shutil.which('cmake')
    if exe:
        return exe
    for c in (glob.glob(os.path.join(TOOLCACHE, 'cmake-*', 'bin', 'cmake'))
              + glob.glob(os.path.join(TOOLCACHE, 'cmake-*', 'bin', 'cmake.exe'))          # Windows
              + glob.glob(os.path.join(TOOLCACHE, 'cmake-*', 'CMake.app', 'Contents', 'bin', 'cmake'))):
        return c
    return None


def _gtest_dir():
    """GOOGLETEST_DIR 优先; 否则用 Tools/.toolcache 里下载固定的 googletest (离线)。"""
    d = os.environ.get('GOOGLETEST_DIR')
    if d and os.path.isdir(d):
        return d
    g = sorted(glob.glob(os.path.join(TOOLCACHE, 'googletest-*')))
    return g[0] if g else None


def run(cmd, **kw):
    print('[gtest] $', ' '.join(cmd))
    return subprocess.call(cmd, **kw)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--project', default=os.environ.get('PROJECT', 'Demo_Tc387'))
    ap.add_argument('--module', default='all', help='GTEST_MODULE (默认 all)')
    a = ap.parse_args()

    build_dir = os.path.join(ROOT, 'build_test')
    cov_dir = os.path.join(ROOT, 'coverage_output')
    report_dir = os.path.join(ROOT, 'Projects', a.project, 'Reports')
    ut_dir = os.path.join(ROOT, 'Projects', a.project, 'Test', 'ut')
    for d in (build_dir, cov_dir, report_dir):
        os.makedirs(d, exist_ok=True)

    cmake = _cmake()
    if not cmake:
        print('[gtest] 未找到 cmake (系统或 Tools/.toolcache); 跑 python Tools/fetch_tools.py 下载', file=sys.stderr)
        return 127
    ctest_exe = shutil.which('ctest') or os.path.join(
        os.path.dirname(cmake), 'ctest' + ('.exe' if cmake.endswith('.exe') else ''))
    if not os.path.isdir(ut_dir) or not os.path.exists(os.path.join(ut_dir, 'CMakeLists.txt')):
        print(f'[gtest] SKIP: {a.project} 无单元测试 ({ut_dir})')
        return 0   # 无测试视为跳过 (多项目矩阵下不阻塞)

    cfg = [cmake, '-S', ut_dir, '-B', build_dir, '-DCMAKE_BUILD_TYPE=Coverage']
    # 离线/固定: 用本地 googletest 源码 (Tools/.toolcache 或 GOOGLETEST_DIR), FetchContent 不联网
    gtdir = _gtest_dir()
    if gtdir:
        cfg.append('-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=' + os.path.abspath(gtdir))
    rc = run(cfg)
    if rc:
        return rc
    rc = run([cmake, '--build', build_dir, '-j', str(os.cpu_count() or 2)])
    if rc:
        return rc
    junit = os.path.join(report_dir, 'gtest_results.xml')
    ctest = [ctest_exe, '--test-dir', build_dir, '--output-on-failure', '--output-junit', junit]
    if a.module and a.module != 'all':
        ctest += ['-R', a.module]        # 按名字过滤 (如 --module Adc)
    rc = run(ctest)

    # 覆盖率 (best-effort, 跨平台)
    if shutil.which('gcovr'):
        run(['gcovr', '-r', ROOT, '--html', '--html-details',
             '-o', os.path.join(cov_dir, 'index.html')])
    elif shutil.which('OpenCppCoverage'):
        run(['OpenCppCoverage', f'--export_type=html:{os.path.join(cov_dir, "html")}',
             '--working_dir', build_dir, '--', 'ctest', '--test-dir', build_dir])
    else:
        print('[gtest] 未找到 gcovr/OpenCppCoverage, 跳过覆盖率')

    print(f'[gtest] 完成。报告: {junit}')
    return rc


if __name__ == '__main__':
    sys.exit(main())
