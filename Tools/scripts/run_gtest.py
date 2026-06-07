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

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))


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

    if not shutil.which('cmake'):
        print('[gtest] 未找到 cmake, 无法构建单元测试', file=sys.stderr)
        return 127
    if not os.path.isdir(ut_dir):
        print(f'[gtest] 未找到单元测试目录: {ut_dir}', file=sys.stderr)
        return 1

    rc = run(['cmake', '-S', ut_dir, '-B', build_dir,
              '-DCMAKE_BUILD_TYPE=Coverage', f'-DGTEST_MODULE={a.module}'])
    if rc:
        return rc
    rc = run(['cmake', '--build', build_dir, '-j', str(os.cpu_count() or 2)])
    if rc:
        return rc
    junit = os.path.join(report_dir, 'gtest_results.xml')
    rc = run(['ctest', '--test-dir', build_dir, '--output-on-failure', '--output-junit', junit])

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
