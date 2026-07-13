#!/usr/bin/env python3
"""scons check 的实现: 静态检查 (cppcheck 真实 bug 门禁 + MISRA 资讯) + 单元测试。

门禁策略 (避免 demo 噪声同时不漏真问题):
  - cppcheck warning/performance/portability  -> 硬门禁 (有发现即失败, 报告 Reports/cppcheck.xml)
  - cppcheck MISRA (--addon=misra)            -> 资讯 (报告 Reports/misra.xml, 不失败; --strict-misra 可转门禁)
  - GoogleTest 单元测试 (run_gtest.py)         -> 硬门禁 (失败即失败)

用法:
    python Tools/scripts/run_check.py [--project Demo_Tc387] [--strict-misra] [--no-tests]
                                      [--module CDD/CddPwm ...] [--path CDD/CddPwm/src/CddPwm.c ...]
                                      [--changed-only] [--html] [--open-report]
                                      [--config-only] [--jobs N] [--require-cppcheck]

配置职责:
  - Projects/<P>/misra.json (回退仓库根 misra.json): include / suppressPaths / deviations / ruleTexts
  - Projects/<P>/build.yaml: global_includes / global_defines / 模块清单 / release_includes 参与上下文补全
  - build_check/cppcheck_effective_config.json: 运行前生成的有效配置快照 (排查 include/define 是否齐)
"""
import argparse
import glob
import html
import json
import os
import shutil
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
# 每层的公共头 (回退用; 正常从 build.yaml 补全)
FALLBACK_HDRS = ('MCAL/inc', 'Components/Common/inc', 'BSW/inc', 'ASW/inc')
GATE_SEVERITIES = ('error', 'warning', 'performance', 'portability')


# ───────────────────────── cppcheck 探测 ─────────────────────────

def bundled_cppcheck():
    matches = glob.glob(os.path.join(ROOT, 'Tools', 'Offline', 'tools', 'windows-x86_64', 'cppcheck', '**', 'cppcheck.exe'), recursive=True)
    return matches[0] if matches else None


def find_cppcheck():
    return shutil.which('cppcheck') or bundled_cppcheck()


def cppcheck_version(exe):
    try:
        out = subprocess.run([exe, '--version'], capture_output=True, text=True, timeout=8)
        return (out.stdout or out.stderr).strip().splitlines()[0]
    except Exception:
        return ''


# ───────────────────────── 配置加载 ─────────────────────────

def _expand_pattern(p):
    """misra.json 的路径模式 -> 实际目录列表 (去掉尾部 /**, 中段 * 用 glob 展开)。"""
    p = p.replace('/**', '').rstrip('/')
    if '*' in p:
        return sorted(os.path.relpath(m, ROOT).replace('\\', '/')
                      for m in glob.glob(os.path.join(ROOT, p)) if os.path.isdir(m))
    return [p]


def _expand_patterns(patterns):
    out = []
    for p in patterns:
        out.extend(_expand_pattern(p))
    return out


def misra_json_path(project):
    """项目级优先, 回退仓库根 (与 USER_MANUAL 一致)。"""
    proj = os.path.join(ROOT, 'Projects', project, 'misra.json')
    return proj if os.path.exists(proj) else os.path.join(ROOT, 'misra.json')


def load_misra(project):
    f = misra_json_path(project)
    if not os.path.exists(f):
        return {}, f
    with open(f, encoding='utf-8') as fh:
        return json.load(fh), f


def load_build_config(project):
    """复用 v3 框架 config_loader 解析 build.yaml (失败时返回 None, 走回退)。"""
    yaml_path = os.path.join(ROOT, 'Projects', project, 'build.yaml')
    if not os.path.exists(yaml_path):
        return None
    sys.path.insert(0, os.path.join(ROOT, 'Tools'))
    try:
        from site_scons import config_loader
        project_dir = os.path.join(ROOT, 'Projects', project)
        variables = {
            'ROOT_DIR': ROOT,
            'BSW_DIR': os.path.join(ROOT, 'BSW'),
            'MCAL_DIR': os.path.join(ROOT, 'MCAL'),
            'COMP_DIR': os.path.join(ROOT, 'Components'),
            'PROJECT_DIR': project_dir,
            'GEN_DIR': os.path.join(project_dir, 'Gen'),
        }
        return config_loader.load(yaml_path, variables)
    except Exception as exc:
        print(f'[check] warn: build.yaml 解析失败, 上下文补全降级: {exc}')
        return None


def module_dir(layer, mod, chip):
    """模块名 -> 工作区目录 (MCAL 带芯片一级; Components 支持嵌套名)。"""
    base = {'MCAL': os.path.join('MCAL', chip)}.get(layer, layer)
    return os.path.join(ROOT, base, *mod.split('/'))


def collect_context(project, cfg, misra):
    """合并去重: misra.json 显式配置 + build.yaml 全局/模块 include 与 define。

    返回 (include_dirs, defines, scan_roots, suppress_paths)。均为仓库相对路径。
    """
    project_dir = os.path.join(ROOT, 'Projects', project)
    inc, defines = [], []
    seen = set()

    def add_inc(path):
        if not path:
            return
        ap = os.path.abspath(path if os.path.isabs(path) else os.path.join(ROOT, path))
        rp = os.path.relpath(ap, ROOT)
        if rp not in seen and os.path.isdir(ap):
            seen.add(rp)
            inc.append(rp)

    # 1) 各层公共头 (回退清单, 始终尝试)
    for h in FALLBACK_HDRS:
        add_inc(h)

    if cfg is not None:
        defines.extend(cfg.global_defines)
        for g in cfg.global_includes:
            add_inc(g)
        chip = cfg.chip or ''
        for layer, mods in cfg.layers.items():
            for name, ov in mods.items():
                mdir = module_dir(layer, name, chip)
                for sub in ('inc', 'include', 'gen', 'cfg'):
                    add_inc(os.path.join(mdir, sub))
                for extra in list(ov.includes) + list(ov.public_includes) + list(ov.generated_include_dirs):
                    add_inc(extra)
                for rel in ov.release_includes:  # 逐文件 -> 所在目录
                    p = rel if os.path.isabs(rel) else os.path.join(ROOT, rel)
                    add_inc(os.path.dirname(p))
        # 项目自身的配置/生成/集成头
        add_inc(os.path.join(project_dir, 'Cfg'))
        add_inc(os.path.join(project_dir, 'Gen'))
        add_inc(os.path.join(project_dir, cfg.integration_dir, 'inc'))

    scan_roots = _expand_patterns(misra.get('include', []))
    scan_roots = [p for p in scan_roots if os.path.isdir(os.path.join(ROOT, p))] or ['ASW', 'CDD', 'Components', 'MCAL']
    suppress = _expand_patterns(misra.get('suppressPaths', []))
    return inc, defines, scan_roots, suppress


# ───────────────────────── 扫描范围解析 ─────────────────────────

def resolve_scan_paths(a, cfg, scan_roots):
    """--module/--path/--changed-only -> 具体扫描输入; 默认整 include 范围。"""
    chip = cfg.chip if cfg is not None else ''
    paths = []
    for m in a.module or []:
        layer, _, mod = m.partition('/')
        d = module_dir(layer, mod, chip) if mod else os.path.join(ROOT, layer)
        if not os.path.isdir(d):
            raise SystemExit(f'[check] fatal: 模块目录不存在: {m} -> {os.path.relpath(d, ROOT)}')
        paths.append(os.path.relpath(d, ROOT))
    for p in a.path or []:
        ap = p if os.path.isabs(p) else os.path.join(ROOT, p)
        if not os.path.exists(ap):
            raise SystemExit(f'[check] fatal: 路径不存在: {p}')
        paths.append(os.path.relpath(ap, ROOT))
    if a.changed_only:
        changed = git_changed_files(scan_roots)
        if not changed:
            print('[check] --changed-only: 扫描范围内无 .c/.h 改动, 跳过静态检查')
            return []
        paths.extend(changed)
    return paths or scan_roots


def git_changed_files(scan_roots):
    def run_git(args):
        r = subprocess.run(['git'] + args, capture_output=True, text=True, cwd=ROOT)
        return r.stdout.splitlines() if r.returncode == 0 else []
    files = set(run_git(['diff', '--name-only', 'HEAD']))
    files.update(run_git(['ls-files', '--others', '--exclude-standard']))
    out = []
    for f in sorted(files):
        if not f.lower().endswith(('.c', '.h')):
            continue
        if any(f == r or f.startswith(r.rstrip('/') + '/') for r in scan_roots):
            if os.path.exists(os.path.join(ROOT, f)):
                out.append(f)
    return out


# ───────────────────────── 报告 ─────────────────────────

def parse_xml_errors(xml_path):
    """从 cppcheck --xml 报告提取 error 条目 (无第三方依赖的轻量解析)。"""
    try:
        import xml.etree.ElementTree as ET
        tree = ET.parse(xml_path)
    except Exception:
        return []
    out = []
    for e in tree.iter('error'):
        loc = e.find('location')
        out.append({
            'id': e.get('id', ''),
            'severity': e.get('severity', ''),
            'msg': e.get('msg', ''),
            'file': loc.get('file', '') if loc is not None else '',
            'line': loc.get('line', '') if loc is not None else '',
        })
    return out


def write_html_report(report_dir, meta, gate_errors, misra_errors):
    out_dir = os.path.join(report_dir, 'static_check')
    os.makedirs(out_dir, exist_ok=True)
    out = os.path.join(out_dir, 'index.html')

    def table(rows):
        if not rows:
            return '<p class="ok">无发现 ✔</p>'
        tr = ''.join(
            f'<tr><td>{html.escape(r["severity"])}</td><td>{html.escape(r["id"])}</td>'
            f'<td>{html.escape(r["file"])}:{html.escape(str(r["line"]))}</td>'
            f'<td>{html.escape(r["msg"])}</td></tr>' for r in rows)
        return ('<div class="wrap"><table><thead><tr><th>级别</th><th>ID</th><th>位置</th><th>说明</th></tr>'
                f'</thead><tbody>{tr}</tbody></table></div>')

    meta_rows = ''.join(f'<tr><td>{html.escape(k)}</td><td>{html.escape(str(v))}</td></tr>' for k, v in meta.items())
    doc = f"""<!DOCTYPE html>
<html lang="zh"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>静态检查报告 - {html.escape(meta.get('项目', ''))}</title>
<style>
 body {{ font-family: system-ui, "Segoe UI", sans-serif; margin: 2rem auto; max-width: 72rem; padding: 0 1rem;
        color: #1a202c; background: #fff; }}
 h1 {{ font-size: 1.4rem; }} h2 {{ font-size: 1.1rem; margin-top: 2rem; }}
 table {{ border-collapse: collapse; width: 100%; font-size: .88rem; }}
 th, td {{ border: 1px solid #cbd5e0; padding: .3rem .55rem; text-align: left; vertical-align: top; }}
 th {{ background: #edf2f7; }}
 .wrap {{ overflow-x: auto; }}
 .ok {{ color: #2f855a; font-weight: 600; }}
 .stat {{ display: inline-block; margin-right: 1.5rem; padding: .4rem .8rem; background: #edf2f7; border-radius: .4rem; }}
 .stat b {{ font-size: 1.3rem; }}
 @media (prefers-color-scheme: dark) {{
   body {{ color: #e2e8f0; background: #1a202c; }}
   th {{ background: #2d3748; }} th, td {{ border-color: #4a5568; }}
   .stat {{ background: #2d3748; }} .ok {{ color: #68d391; }}
 }}
</style></head><body>
<h1>静态检查报告</h1>
<div class="wrap"><table><tbody>{meta_rows}</tbody></table></div>
<p>
 <span class="stat">门禁发现 <b>{len(gate_errors)}</b></span>
 <span class="stat">MISRA 发现 <b>{len(misra_errors)}</b></span>
</p>
<h2>1 · 真实 bug 门禁 (warning / performance / portability)</h2>
{table(gate_errors)}
<h2>2 · MISRA C:2012 ({html.escape(meta.get('MISRA 模式', '资讯'))})</h2>
{table(misra_errors)}
</body></html>
"""
    with open(out, 'w', encoding='utf-8') as f:
        f.write(doc)
    return out


# ───────────────────────── 主流程 ─────────────────────────

def run(cmd, **kw):
    print('[check] $', ' '.join(cmd))
    return subprocess.run(cmd, **kw)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--project', default=os.environ.get('PROJECT', 'Demo_Tc387'))
    ap.add_argument('--strict-misra', action='store_true', help='MISRA 违规也作为门禁')
    ap.add_argument('--no-tests', action='store_true', help='只做静态检查, 不跑单测')
    ap.add_argument('--module', action='append', metavar='Layer/Mod', help='只检查指定模块目录 (可重复)')
    ap.add_argument('--path', action='append', metavar='PATH', help='只检查指定文件/目录 (可重复)')
    ap.add_argument('--changed-only', action='store_true', help='只检查 git 改动的 .c/.h (含未跟踪)')
    ap.add_argument('--html', action='store_true', help='生成 Reports/static_check/index.html 汇总报告')
    ap.add_argument('--open-report', action='store_true', help='运行后打开 HTML 报告')
    ap.add_argument('--config-only', action='store_true', help='只生成有效配置快照, 不执行 cppcheck')
    ap.add_argument('--jobs', type=int, default=0, metavar='N', help='cppcheck 并行度 (默认 CPU 数)')
    ap.add_argument('--require-cppcheck', action='store_true', help='找不到 cppcheck 时失败 (rc=3) 而非跳过')
    a = ap.parse_args()

    project_dir = os.path.join(ROOT, 'Projects', a.project)
    report_dir = os.path.join(project_dir, 'Reports')
    os.makedirs(report_dir, exist_ok=True)
    build_dir = os.path.join(ROOT, 'build_check')  # 分析缓存 (避免 ctu-info 落进源码树)
    os.makedirs(build_dir, exist_ok=True)

    misra, misra_file = load_misra(a.project)
    cfg = load_build_config(a.project)
    inc, defines, scan_roots, suppress = collect_context(a.project, cfg, misra)
    scan_paths = resolve_scan_paths(a, cfg, scan_roots)

    cppcheck = find_cppcheck()
    rule_texts = misra.get('ruleTexts', '')
    if rule_texts and not os.path.isabs(rule_texts):
        rule_texts = os.path.join(ROOT, rule_texts)

    # 有效配置快照 (排查 include/define/范围是否正确)
    effective = {
        'project': a.project,
        'cppcheck': cppcheck or '(not found)',
        'cppcheckVersion': cppcheck_version(cppcheck) if cppcheck else '',
        'configSources': {
            'misraJson': os.path.relpath(misra_file, ROOT),
            'buildYaml': f'Projects/{a.project}/build.yaml' if cfg is not None else '(未解析, 用回退)',
        },
        'includePaths': inc,
        'defines': defines,
        'scanPaths': scan_paths,
        'excludePaths': suppress,
        'misraRuleTexts': os.path.relpath(rule_texts, ROOT) if rule_texts else '',
    }
    eff_path = os.path.join(build_dir, 'cppcheck_effective_config.json')
    with open(eff_path, 'w', encoding='utf-8') as f:
        json.dump(effective, f, ensure_ascii=False, indent=2)
    print(f'[check] 有效配置 -> {os.path.relpath(eff_path, ROOT)}')
    if a.config_only:
        return 0
    if not scan_paths:  # --changed-only 且无改动
        return 0

    rc = 0
    gate_errors, misra_errors = [], []
    gate_xml = os.path.join(report_dir, 'cppcheck.xml')
    misra_xml = os.path.join(report_dir, 'misra.xml')

    if not cppcheck:
        print('[check] ' + '!' * 62)
        print('[check] !! 未找到 cppcheck, 静态检查被跳过 (Linux: apt install cppcheck;')
        print('[check] !! Windows: python Tools/setup_env.py --offline 用内置离线包)')
        print('[check] ' + '!' * 62)
        if a.require_cppcheck:
            print('[check] FAIL: --require-cppcheck 生效, 缺 cppcheck 视为失败')
            return 3
    else:
        jobs = a.jobs or os.cpu_count() or 1
        common = ['--inline-suppr', '--quiet', f'-j{jobs}', f'--cppcheck-build-dir={build_dir}'] \
                 + [f'-D{d}' for d in defines] + [f'-I{h}' for h in inc] \
                 + [f'-i{s}' for s in suppress] + scan_paths
        # 1) 真实 bug 门禁
        print('[check] cppcheck 真实 bug 门禁 (warning/performance/portability)')
        run([cppcheck, '--enable=warning,performance,portability',
             '--xml', '--output-file=' + gate_xml] + common, cwd=ROOT)
        gate_errors = [e for e in parse_xml_errors(gate_xml) if e['severity'] in GATE_SEVERITIES]
        if gate_errors:
            print(f'[check] FAIL: cppcheck 发现 {len(gate_errors)} 个问题 (报告 {os.path.relpath(gate_xml, ROOT)})')
            rc = 2
        else:
            print('[check] PASS: cppcheck warning/performance/portability gate')
        # 2) MISRA (资讯, 写报告)
        print('[check] cppcheck MISRA (--addon=misra, ' + ('门禁' if a.strict_misra else '资讯') + ')')
        addon = 'misra'
        if rule_texts and os.path.exists(rule_texts):
            addon_json = os.path.join(build_dir, 'misra_addon.json')
            with open(addon_json, 'w', encoding='utf-8') as f:
                json.dump({'script': 'misra', 'args': [f'--rule-texts={rule_texts}']}, f)
            addon = addon_json
        run([cppcheck, f'--addon={addon}', '--enable=style', '--xml',
             '--output-file=' + misra_xml] + common, cwd=ROOT)
        misra_errors = parse_xml_errors(misra_xml)
        print(f'[check] MISRA 发现 {len(misra_errors)} 条 (报告 {os.path.relpath(misra_xml, ROOT)})')
        if a.strict_misra and misra_errors:
            rc = rc or 1

    # 3) HTML 汇总报告
    if a.html or a.open_report:
        def _git(args):
            r = subprocess.run(['git'] + args, capture_output=True, text=True, cwd=ROOT)
            return r.stdout.strip() if r.returncode == 0 else ''
        meta = {
            '项目': a.project,
            'commit': _git(['rev-parse', '--short', 'HEAD']) + (' (dirty)' if _git(['status', '--porcelain']) else ''),
            '检查时间': subprocess.run([sys.executable, '-c', 'import datetime;print(datetime.datetime.now().isoformat(timespec="seconds"))'],
                                   capture_output=True, text=True).stdout.strip(),
            '工具': effective['cppcheckVersion'] or '(cppcheck 缺失)',
            '扫描范围': ', '.join(scan_paths),
            '排除': ', '.join(suppress) or '(无)',
            '配置来源': f"{effective['configSources']['misraJson']} + {effective['configSources']['buildYaml']}",
            'MISRA 模式': '严格门禁' if a.strict_misra else '资讯',
        }
        report = write_html_report(report_dir, meta, gate_errors, misra_errors)
        print(f'[check] HTML 报告 -> {os.path.relpath(report, ROOT)}')
        if a.open_report:
            import webbrowser
            webbrowser.open('file://' + os.path.abspath(report))

    # 4) 单元测试
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
