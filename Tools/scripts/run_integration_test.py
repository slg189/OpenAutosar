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
sys.path.insert(0, os.path.join(ROOT, 'Tools'))   # 框架为 Tools/site_scons 包
CONFIG = 'TestPlatform_UintTest_QEMU_Test.json'


def _app_name(project, config):
    """从 build.yaml 推导工程产物基名 (ELF 名); 失败时回退为工程目录名。"""
    try:
        from site_scons import config_loader
        cfg = config_loader.load(os.path.join(ROOT, 'Projects', project, config))
        return config_loader.app_name(cfg)
    except Exception as exc:   # noqa: BLE001 (脚本兜底)
        print(f'[itest] 警告: 读取 build.yaml 失败 ({exc}), ELF 名回退为 {project}', file=sys.stderr)
        return project


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--project', default=os.environ.get('PROJECT', 'Demo_Tc387'))
    ap.add_argument('--config', default='build.yaml', help='build.yaml 文件名 (产物名来源)')
    a = ap.parse_args()

    report_dir = os.path.join(ROOT, 'Projects', a.project, 'Reports')
    os.makedirs(report_dir, exist_ok=True)

    # 1) 构建工程 (v3: 仓库根 scons, 目标工具链/平台均取自 build.yaml)
    scons = shutil.which('scons') or 'scons'
    print(f'[itest] 构建工程 {a.project} ...')
    rc = subprocess.call([scons, f'PROJECT=Projects/{a.project}', f'CONFIG={a.config}',
                          f'-j{os.cpu_count() or 2}'], cwd=ROOT)
    if rc:
        print(f'[itest] 构建失败 rc={rc} (目标 ELF 需 TriCore 工具链)', file=sys.stderr)
        return rc

    app = _app_name(a.project, a.config)   # 从 build.yaml 取产物名, 不假设 = 工程目录名
    elf = os.path.join(ROOT, 'Projects', a.project, 'Out', f'{app}.elf')
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
