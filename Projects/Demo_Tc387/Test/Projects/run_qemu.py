#!/usr/bin/env python3
"""在 QEMU (tricore) 中加载 ELF 运行集成测试, 解析半主机输出, 写 JUnit 报告。

由 Tools/scripts/run_integration_test.py 调用, 也可单独使用:
    python run_qemu.py --config <json> --elf <elf> --report <xml>

判定: 半主机输出中出现 "ALL TESTS PASSED" / "TEST_PASS" 视为通过;
      出现 "TEST_FAIL" 或 qemu 非零退出视为失败; 超时视为失败。
qemu 或 ELF 缺失时记为 skipped 并返回 0 (不阻塞无 TriCore 环境的 CI)。
"""
import argparse
import json
import os
import shutil
import subprocess
import sys
import time
import xml.sax.saxutils as sx


def write_junit(path, name, status, time_s=0.0, stdout='', message=''):
    """status: passed | failed | skipped"""
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
    failures = 1 if status == 'failed' else 0
    skipped = 1 if status == 'skipped' else 0
    inner = ''
    if status == 'failed':
        inner = f'<failure message={sx.quoteattr(message or "failed")}/>'
    elif status == 'skipped':
        inner = f'<skipped message={sx.quoteattr(message or "skipped")}/>'
    with open(path, 'w', encoding='utf-8') as f:
        f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
        f.write(f'<testsuites tests="1" failures="{failures}" skipped="{skipped}">\n')
        f.write(f'  <testsuite name="qemu_integration" tests="1" '
                f'failures="{failures}" skipped="{skipped}" time="{time_s:.3f}">\n')
        f.write(f'    <testcase name={sx.quoteattr(name)} time="{time_s:.3f}">{inner}'
                f'<system-out>{sx.escape(stdout[-4000:])}</system-out></testcase>\n')
        f.write('  </testsuite>\n</testsuites>\n')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--config', required=True)
    ap.add_argument('--elf', required=True)
    ap.add_argument('--report', required=True)
    a = ap.parse_args()

    cfg = {}
    if os.path.exists(a.config):
        with open(a.config, encoding='utf-8') as f:
            cfg = json.load(f)
    machine = cfg.get('machine', 'tricore-aurix-tc3xx')
    cpu = cfg.get('cpu', 'tc387')
    timeout_s = int(cfg.get('timeoutSec', 300))
    qemu = (os.environ.get('QEMU_TRICORE') or shutil.which('qemu-system-tricore'))

    # 缺 qemu 或 ELF -> 跳过 (无 TriCore 环境时不阻塞)
    if not qemu:
        msg = '未找到 qemu-system-tricore (设 QEMU_TRICORE 或安装后重试)'
        print(f'[qemu] SKIP: {msg}')
        write_junit(a.report, 'boot_and_run', 'skipped', message=msg)
        return 0
    if not os.path.exists(a.elf):
        msg = f'未找到 ELF: {a.elf} (需 TriCore 工具链先构建)'
        print(f'[qemu] SKIP: {msg}')
        write_junit(a.report, 'boot_and_run', 'skipped', message=msg)
        return 0

    cmd = [qemu, '-M', machine, '-cpu', cpu, '-nographic', '-no-reboot',
           '-semihosting-config', 'enable=on,target=native',
           '-kernel', a.elf]
    print('[qemu] $', ' '.join(cmd))
    t0 = time.time()
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout_s)
        out = (p.stdout or '') + (p.stderr or '')
        rc = p.returncode
    except subprocess.TimeoutExpired as e:
        out = e.stdout if isinstance(e.stdout, str) else ''
        print(f'[qemu] 超时 {timeout_s}s')
        write_junit(a.report, 'boot_and_run', 'failed', time.time() - t0, out, '超时')
        return 1
    dt = time.time() - t0

    passed = ('ALL TESTS PASSED' in out) or ('TEST_PASS' in out)
    failed = ('TEST_FAIL' in out) or (rc != 0)
    if passed and not failed:
        print(f'[qemu] PASS ({dt:.2f}s)')
        write_junit(a.report, 'boot_and_run', 'passed', dt, out)
        return 0
    print(f'[qemu] FAIL (rc={rc})\n{out[-2000:]}')
    write_junit(a.report, 'boot_and_run', 'failed', dt, out, f'rc={rc}')
    return 1


if __name__ == '__main__':
    sys.exit(main())
