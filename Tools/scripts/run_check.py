#!/usr/bin/env python3
"""scons check 的实现: 静态检查 (cppcheck 真实 bug 门禁 + MISRA 资讯) + 单元测试。

门禁策略 (避免 demo 噪声同时不漏真问题):
  - cppcheck warning/performance/portability  -> 硬门禁 (有发现即失败)
  - cppcheck MISRA (--addon=misra)            -> 资讯 (打印数量/写报告, 不失败; --strict-misra 可转门禁)
  - GoogleTest 单元测试 (run_gtest.py)         -> 硬门禁 (失败即失败)

用法:
    python Tools/scripts/run_check.py [--project Demo_Tc387] [--strict-misra] [--no-tests]
范围/抑制取自仓库根 misra.json (include / suppressPaths)。
"""
import argparse
import json
import os
import shutil
import subprocess
import sys
import glob

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))


def bundled_cppcheck():
    matches = glob.glob(os.path.join(ROOT, 'Tools', 'Offline', 'tools', 'windows-x86_64', 'cppcheck', '**', 'cppcheck.exe'), recursive=True)
    return matches[0] if matches else None


def _strip_glob(p):
    return p.replace('/**', '').replace('/*', '').rstrip('/')


def load_misra():
    f = os.path.join(ROOT, 'misra.json')
    if not os.path.exists(f):
        return [], []
    d = json.load(open(f, encoding='utf-8'))
    inc = [_strip_glob(x) for x in d.get('include', [])]
    sup = [_strip_glob(x) for x in d.get('suppressPaths', [])]
    return inc, sup


def run(cmd, **kw):
    print('[check] $', ' '.join(cmd))
    return subprocess.run(cmd, **kw)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--project', default=os.environ.get('PROJECT', 'Demo_Tc387'))
    ap.add_argument('--strict-misra', action='store_true', help='MISRA 违规也作为门禁')
    ap.add_argument('--no-tests', action='store_true', help='只做静态检查, 不跑单测')
    a = ap.parse_args()

    inc_paths, suppress = load_misra()
    inc_paths = [p for p in inc_paths if os.path.isdir(os.path.join(ROOT, p))] or ['ASW', 'CDD', 'Components', 'MCAL']
    # cppcheck 解析用的头文件目录
    hdr = [d for d in ('MCAL/inc', 'Components/Common/inc', 'BSW/inc', 'ASW/inc')
           if os.path.isdir(os.path.join(ROOT, d))]
    report_dir = os.path.join(ROOT, 'Projects', a.project, 'Reports')
    os.makedirs(report_dir, exist_ok=True)
    # cppcheck 分析缓存目录 (避免 MISRA addon 的 ctu-info 文件落到源码树)
    build_dir = os.path.join(ROOT, 'build_check')
    os.makedirs(build_dir, exist_ok=True)

    rc = 0
    cppcheck = shutil.which('cppcheck') or bundled_cppcheck()
    if not cppcheck:
        print('[check] 未找到 cppcheck, 跳过静态检查 (apt install cppcheck)')
    else:
        common = ['--inline-suppr', '--quiet', f'--cppcheck-build-dir={build_dir}'] \
                 + [f'-I{h}' for h in hdr] + [f'-i{s}' for s in suppress] + inc_paths
        # 1) 真实 bug 门禁
        print('[check] cppcheck 真实 bug 门禁 (warning/performance/portability)')
        r = run([cppcheck, '--enable=warning,performance,portability',
                 '--error-exitcode=2'] + common, cwd=ROOT)
        if r.returncode != 0:
            print('[check] FAIL: cppcheck found issues')
            rc = 2
        else:
            print('[check] PASS: cppcheck warning/performance/portability gate')
        # 2) MISRA (资讯, 写报告)
        print('[check] cppcheck MISRA (--addon=misra, 资讯)')
        misra_xml = os.path.join(report_dir, 'misra.xml')
        ec = ['--error-exitcode=1'] if a.strict_misra else []
        r2 = run([cppcheck, '--addon=misra', '--enable=style', '--xml',
                  '--output-file=' + misra_xml] + ec + common, cwd=ROOT)
        n = 0
        try:
            import re
            n = len(re.findall(r'<error ', open(misra_xml, encoding='utf-8').read()))
        except OSError:
            pass
        print(f'[check] MISRA 发现 {n} 条 (报告 {os.path.relpath(misra_xml, ROOT)})'
              + ('' if not a.strict_misra else f' [strict: rc={r2.returncode}]'))
        if a.strict_misra and r2.returncode != 0:
            rc = rc or 1

    # 3) 单元测试
    if not a.no_tests:
        print('[check] GoogleTest 单元测试')
        rt = subprocess.call([sys.executable, os.path.join(ROOT, 'Tools', 'scripts', 'run_gtest.py'),
                              '--project', a.project])
        if rt != 0:
            print('[check] FAIL: unit tests failed')
            rc = rc or rt

    print(f'[check] 完成, rc={rc}')
    return rc


if __name__ == '__main__':
    sys.exit(main())
