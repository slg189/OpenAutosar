#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""在 QEMU 上运行集成测试 (跨平台, 取代 run_integrationTest.bat)。

流程: 用 v3 构建项目 ELF -> 调 Projects/<P>/Test/Projects/run_qemu.py 运行。
用法:
    python Tools/scripts/run_integration_test.py [--project Demo_Tc387]
注: 构建目标 ELF 需 TriCore 工具链 (hightec/tasking); 无工具链时会在构建步失败。
"""
import argparse
import os
import shutil
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
CONFIG = 'TestPlatform_UintTest_QEMU_Test.json'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--project', default=os.environ.get('PROJECT', 'Demo_Tc387'))
    a = ap.parse_args()

    report_dir = os.path.join(ROOT, 'Projects', a.project, 'Reports')
    os.makedirs(report_dir, exist_ok=True)

    # 1) 构建工程 (v3: 仓库根 scons, 默认目标工具链 hightec/AURIX2G)
    scons = shutil.which('scons') or 'scons'
    print(f'[itest] 构建工程 {a.project} ...')
    rc = subprocess.call([scons, f'PROJECT=Projects/{a.project}',
                          f'-j{os.cpu_count() or 2}'], cwd=ROOT)
    if rc:
        print(f'[itest] 构建失败 rc={rc} (目标 ELF 需 TriCore 工具链)', file=sys.stderr)
        return rc

    elf = os.path.join(ROOT, 'Projects', a.project, 'Out', f'{a.project}.elf')
    if not os.path.exists(elf):
        print(f'[itest] 未找到 ELF: {elf}', file=sys.stderr)
        return 1

    # 2) 在 QEMU 中运行
    runner = os.path.join(ROOT, 'Projects', a.project, 'Test', 'Projects', 'run_qemu.py')
    report = os.path.join(report_dir, f'integration_{a.project}.xml')
    print('[itest] 在 QEMU 中运行集成测试 ...')
    rc = subprocess.call([sys.executable, runner, '--config', os.path.join(ROOT, CONFIG),
                          '--elf', elf, '--report', report])
    if rc == 0:
        print(f'[itest] 完成。报告: {report}')
    return rc


if __name__ == '__main__':
    sys.exit(main())
