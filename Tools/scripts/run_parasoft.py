#!/usr/bin/env python3
"""Parasoft C/C++test 静态检查入口 (cpptestcli 命令行包装)。

读取 Projects/<P>/parasoft.json, 按 edition 拼装 cpptestcli 命令并执行:
  standard     : cpptestcli -compiler <id> -config <cfg> -module . -input <compile_commands.json|bdf>
                 -report <dir> -property report.format=xml,html,sarif [-fail]
  professional : cpptestcli -data <workspace> -config <cfg> -bdf <bdf> -report <dir> [-localsettings f]

用法:
    python Tools/scripts/run_parasoft.py [--project Demo_Tc387] [--dry-run] [--fail]
                                         [--config "builtin://MISRA C 2012"]
                                         [--list-compilers | --list-configs] [--open-report]

设计给两类使用者:
  - AI/CI: 一条命令 + 确定性退出码 (cpptestcli 原始 rc; -fail 时有发现即非零),
           读 Reports/parasoft/report.sarif (SARIF) 即可拿到结构化结果
  - 人:    检查页面 (oa.bat check-ui) 的 Parasoft 区可视化改 parasoft.json 并点击运行

Parasoft 为商业授权软件, 不入仓库; 工具路径按 parasoft.json:toolPath -> 环境变量
CPPTEST_CLI -> PATH 顺序探测 (同 HighTec/ETAS 的处理模式)。许可/DTP 用 localsettings
properties 文件 (license.network.* / dtp.*), 路径填在 parasoft.json:localsettings。
"""
import argparse
import json
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

DEFAULT_CONFIG = {
    'edition': 'standard',
    'toolPath': '',
    'compilerConfig': 'gcc_13-64',
    'testConfig': 'builtin://MISRA C 2012',
    'input': '${PROJECT_DIR}/compile_commands.json',
    'bdf': '',
    'workspace': '',
    'localsettings': '',
    'reportDir': '${PROJECT_DIR}/Reports/parasoft',
    'reportFormats': ['xml', 'html', 'sarif'],
    'failOnFindings': False,
    'options': [],
}

_VAR_RE = re.compile(r'\$\{([A-Z_][A-Z0-9_]*)\}')


def expand(value, variables):
    if isinstance(value, str):
        return _VAR_RE.sub(lambda m: variables.get(m.group(1), m.group(0)), value)
    if isinstance(value, list):
        return [expand(v, variables) for v in value]
    return value


def config_path(project):
    return os.path.join(ROOT, 'Projects', project, 'parasoft.json')


def raw_config(project):
    """parasoft.json 原样内容 (未展开 ${VAR}; 页面编辑用) + 默认值兜底。"""
    cfg = dict(DEFAULT_CONFIG)
    f = config_path(project)
    if os.path.exists(f):
        with open(f, encoding='utf-8') as fh:
            cfg.update(json.load(fh))
    return cfg


def expand_config(project, cfg):
    project_dir = os.path.join(ROOT, 'Projects', project)
    variables = {
        'ROOT_DIR': ROOT,
        'PROJECT_DIR': project_dir,
        'BSW_DIR': os.path.join(ROOT, 'BSW'),
        'MCAL_DIR': os.path.join(ROOT, 'MCAL'),
        'COMP_DIR': os.path.join(ROOT, 'Components'),
        'GEN_DIR': os.path.join(project_dir, 'Gen'),
    }
    return {k: expand(v, variables) for k, v in cfg.items()}


def load_config(project):
    return expand_config(project, raw_config(project))


def find_cpptestcli(cfg):
    """toolPath -> 环境变量 CPPTEST_CLI -> PATH。"""
    for cand in (cfg.get('toolPath', ''), os.environ.get('CPPTEST_CLI', '')):
        if cand and (os.path.exists(cand) or shutil.which(cand)):
            return cand
    return shutil.which('cpptestcli')


def tool_version(exe):
    try:
        out = subprocess.run([exe, '-version'], capture_output=True, text=True, timeout=30)
        return (out.stdout or out.stderr).strip().splitlines()[0]
    except Exception:
        return ''


def build_command(cfg, tool, fail):
    edition = cfg.get('edition', 'standard')
    cmd = [tool]
    if edition == 'professional':
        if cfg.get('workspace'):
            cmd += ['-data', cfg['workspace']]
        cmd += ['-config', cfg['testConfig']]
        if cfg.get('bdf'):
            cmd += ['-bdf', cfg['bdf']]
    else:  # standard / DTP engine
        cmd += ['-compiler', cfg['compilerConfig'], '-config', cfg['testConfig'], '-module', '.']
        cmd += ['-input', cfg.get('bdf') or cfg['input']]
    cmd += ['-report', cfg['reportDir']]
    formats = [f for f in cfg.get('reportFormats', []) if f]
    if formats:
        cmd += ['-property', 'report.format=' + ','.join(formats)]
    if cfg.get('localsettings'):
        cmd += ['-localsettings', cfg['localsettings']]
    if fail:
        cmd.append('-fail')
    cmd += [str(o) for o in cfg.get('options', [])]
    return cmd


def summarize_report(report_dir):
    """解析 report.xml 打印发现数摘要 (尽力而为, 失败不影响退出码)。"""
    xml = os.path.join(report_dir, 'report.xml')
    if not os.path.exists(xml):
        return
    try:
        import xml.etree.ElementTree as ET
        tree = ET.parse(xml)
        viols = tree.findall('.//StdViol') + tree.findall('.//FlowViol') + tree.findall('.//DupViol')
        if not viols:  # 版本差异兜底: 数 sev 属性节点
            viols = [e for e in tree.iter() if e.get('sev') is not None]
        by_sev = {}
        for v in viols:
            by_sev[v.get('sev', '?')] = by_sev.get(v.get('sev', '?'), 0) + 1
        detail = ', '.join(f'sev{k}={n}' for k, n in sorted(by_sev.items())) if by_sev else '无'
        print(f'[parasoft] 发现 {len(viols)} 条 ({detail}); 报告 {os.path.relpath(report_dir, ROOT)}/')
    except Exception as exc:
        print(f'[parasoft] warn: 报告摘要解析失败 ({exc}); 详见 {os.path.relpath(xml, ROOT)}')


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--project', default=os.environ.get('PROJECT', 'Demo_Tc387'))
    ap.add_argument('--dry-run', action='store_true', help='只打印/记录命令, 不执行 (无 Parasoft 环境可验证)')
    ap.add_argument('--fail', action='store_true', help='有静态分析发现即非零退出 (临时覆盖 failOnFindings)')
    ap.add_argument('--config', default='', metavar='TESTCONFIG', help='临时覆盖 testConfig, 如 "builtin://Recommended Rules"')
    ap.add_argument('--list-compilers', action='store_true', help='列出支持的编译器配置 id (透传 cpptestcli)')
    ap.add_argument('--list-configs', action='store_true', help='列出可用测试配置 (透传 cpptestcli)')
    ap.add_argument('--open-report', action='store_true', help='运行后打开 HTML 报告')
    a = ap.parse_args()

    cfg = load_config(a.project)
    if a.config:
        cfg['testConfig'] = a.config
    fail = a.fail or bool(cfg.get('failOnFindings'))

    tool = find_cpptestcli(cfg)
    if not tool and not a.dry_run:
        print('[parasoft] ERROR: 未找到 cpptestcli (Parasoft C/C++test 为商业软件, 需本机安装+授权)', file=sys.stderr)
        print('[parasoft] 配置方式: parasoft.json 的 toolPath, 或环境变量 CPPTEST_CLI, 或加入 PATH', file=sys.stderr)
        return 3

    if a.list_compilers or a.list_configs:
        flag = '-list-compilers' if a.list_compilers else '-list-configs'
        print(f'[parasoft] $ {tool} {flag}')
        return subprocess.call([tool, flag], cwd=ROOT)

    cmd = build_command(cfg, tool or 'cpptestcli', fail)
    report_dir = cfg['reportDir']
    os.makedirs(report_dir, exist_ok=True)
    quoted = subprocess.list2cmdline(cmd)
    with open(os.path.join(report_dir, 'parasoft_command.txt'), 'w', encoding='utf-8') as f:
        f.write(quoted + '\n')

    print(f'[parasoft] edition   : {cfg.get("edition", "standard")}')
    print(f'[parasoft] testConfig: {cfg["testConfig"]}')
    print(f'[parasoft] report    : {os.path.relpath(report_dir, ROOT)}  ({",".join(cfg.get("reportFormats", []))})')
    print(f'[parasoft] command   : {quoted}')

    # Standard 输入检查: compile_commands.json 由 scons 构建生成 (build.yaml output.compile_commands_json)
    scope = cfg.get('bdf') or cfg.get('input', '')
    if cfg.get('edition', 'standard') != 'professional' and scope and not os.path.exists(scope):
        msg = f'[parasoft] 输入 scope 不存在: {os.path.relpath(scope, ROOT)} (先跑 scons 生成 compile_commands.json)'
        if a.dry_run:
            print('[parasoft] WARN: ' + msg)
        else:
            print('[parasoft] ERROR: ' + msg, file=sys.stderr)
            return 2

    if a.dry_run:
        print('[parasoft] dry-run: 命令未执行 (已写 parasoft_command.txt)')
        return 0

    rc = subprocess.call(cmd, cwd=ROOT)
    summarize_report(report_dir)
    if a.open_report:
        html = os.path.join(report_dir, 'report.html')
        if os.path.exists(html):
            import webbrowser
            webbrowser.open('file://' + os.path.abspath(html))
    print(f'[parasoft] 完成, rc={rc}' + (' (含 -fail 门禁)' if fail else ''))
    return rc


if __name__ == '__main__':
    sys.exit(main())
