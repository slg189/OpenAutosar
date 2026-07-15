#!/usr/bin/env python3
"""Vector DaVinci Configurator 命令行代码生成入口 (DVCfgCmd 包装)。

读取 build.yaml 中 `type: vector` 的 codegen generator 作为基底配置, 叠加
Projects/<P>/vector_codegen.json (检查页面写这份, 字段级覆盖, 不动 build.yaml),
CLI 参数最后覆盖; 拼装 DVCfgCmd 命令并执行:

    DVCfgCmd.exe -p <工程.dpa> -g [--genType REAL] [--modules A,B] [-l 日志]

选项名/风格随 DaVinci 版本可调 (project_option/-p, generate_option/-g,
project_arg_style split|joined, ...), 默认值为常见形态; 用 --dry-run 在没有
Vector 环境的机器上核对最终命令。

用法:
    python Tools/scripts/run_vector_davinci.py [--project Demo_Tc387] [--dry-run]
                                               [--generator vector_bsw] [--gen-type REAL]
                                               [--module Can --module CanIf] [--option ...]

设计给两类使用者:
  - AI/CI: 一条命令 + 确定性退出码 (工具 rc 透传; 缺工具真跑 rc=2);
           命令留痕 <output_dir>/davinci_command.txt, 日志 davinci_codegen.log
  - 人:    检查页面 (oa.bat check-ui) 的 Vector 区可视化改配置并点击运行

Vector 为商业授权软件, 不入仓库; 工具路径按 vector_codegen.json/build.yaml 的
tool_path -> 环境变量 DVCFG_CMD -> PATH 顺序探测 (同 HighTec/ETAS/Parasoft 模式)。
"""
import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

_CWD = Path.cwd()
if (_CWD / 'Tools' / 'scripts' / 'run_vector_davinci.py').exists():
    ROOT = _CWD
else:
    ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'Tools'))
venv_site = ROOT / 'Tools' / '.venv' / 'Lib' / 'site-packages'
if venv_site.is_dir():
    sys.path.insert(0, str(venv_site))

from site_scons import config_loader  # noqa: E402

# 页面/CLI 可覆盖的字段 (与 build.yaml generator extra 同名)
OVERRIDE_KEYS = ('tool_path', 'project', 'output_dir', 'gen_type', 'modules', 'log_file',
                 'project_option', 'project_arg_style', 'generate_option', 'output_option',
                 'gen_type_option', 'modules_option', 'log_option', 'options')


def _project_dir(value: str) -> Path:
    p = (value or 'Projects/Demo_Tc387').replace('\\', '/').strip('/')
    if not p.startswith('Projects/'):
        p = 'Projects/' + p
    return (ROOT / p).absolute()


def _as_list(value):
    if value is None:
        return []
    if isinstance(value, list):
        return [str(v) for v in value]
    return [str(value)]


def override_file(project_dir: Path) -> Path:
    return project_dir / 'vector_codegen.json'


def project_variables(project_dir: Path):
    return {
        'ROOT_DIR': str(ROOT),
        'PROJECT_DIR': str(project_dir),
        'BSW_DIR': str(ROOT / 'BSW'),
        'MCAL_DIR': str(ROOT / 'MCAL'),
        'COMP_DIR': str(ROOT / 'Components'),
        'GEN_DIR': str(project_dir / 'Gen'),
    }


def load_merged(project, generator=''):
    """(检查页面复用) 项目名 -> (project_dir, spec, merged, source)。"""
    project_dir = _project_dir(project)
    variables = project_variables(project_dir)
    cfg = config_loader.load(str(project_dir / 'build.yaml'), variables=variables)
    spec = find_spec(cfg, generator)
    merged, source = merged_config(project_dir, spec, variables)
    return project_dir, spec, merged, source


def find_spec(cfg, name: str):
    vec = [s for s in cfg.codegen_specs if s.type == 'vector']
    if name:
        for spec in vec:
            if spec.name == name:
                return spec
        raise SystemExit(f'[vector] generator not found: {name}')
    if not vec:
        raise SystemExit('[vector] build.yaml 中没有 type: vector 的 codegen generator')
    return vec[0]


def _expand(value, variables):
    """vector_codegen.json 里的 ${VAR} 展开 (build.yaml 值已由 config_loader 展开)。"""
    if isinstance(value, str):
        for k, v in variables.items():
            value = value.replace('${' + k + '}', v)
        return value
    if isinstance(value, list):
        return [_expand(v, variables) for v in value]
    return value


def merged_config(project_dir: Path, spec, variables):
    """build.yaml generator (基底) <- vector_codegen.json (页面覆盖层)。"""
    merged = {'tool_path': spec.tool_path}
    merged.update(spec.extra or {})
    ov = override_file(project_dir)
    source = {'base': 'build.yaml', 'override': ''}
    if ov.exists():
        with open(ov, encoding='utf-8') as fh:
            data = json.load(fh)
        merged.update({k: _expand(data[k], variables) for k in OVERRIDE_KEYS if k in data})
        source['override'] = ov.name
    return merged, source


def find_tool(merged, cli_tool=''):
    for cand in (cli_tool, merged.get('tool_path', ''), os.environ.get('DVCFG_CMD', '')):
        if cand:
            return str(cand)
    return 'DVCfgCmd.exe'


def _tool_exists(tool: str) -> bool:
    if tool.lower() in ('cmd', 'cmd.exe'):
        return True
    p = Path(tool)
    if p.is_absolute() or any(sep in tool for sep in ('\\', '/')):
        return p.exists()
    return shutil.which(tool) is not None


def build_command(merged, project_dir: Path, args=None):
    """按合并配置 (+可选 CLI 覆盖) 拼 DVCfgCmd argv。返回 (argv, dpa, output_dir)。"""
    g = merged.get

    def cli(attr, default=''):
        return getattr(args, attr, '') or default

    tool = find_tool(merged, cli('tool'))
    dpa = cli('dpa') or g('project') or str(project_dir / 'Cfg' / 'BSW' / f'{project_dir.name}.dpa')
    output_dir = Path(cli('output') or g('output_dir') or project_dir / 'Gen' / 'Vector')
    gen_type = cli('gen_type') or g('gen_type', '')
    modules = list(getattr(args, 'module', None) or []) or _as_list(g('modules'))
    log_file = g('log_file') or str(output_dir / 'davinci_codegen.log')

    # 空字符串视为"用默认" (页面未填时不至于拼出坏命令); output_option 例外: 空 = 不传
    project_option = str(g('project_option') or '-p')
    project_arg_style = str(g('project_arg_style') or 'split')  # split: -p X, joined: -p=X
    generate_option = str(g('generate_option') or '-g')
    output_option = str(g('output_option') or '')               # DaVinci 输出目录通常定义在 .dpa 内, 默认不传
    gen_type_option = str(g('gen_type_option') or '--genType')
    modules_option = str(g('modules_option') or '--modules')
    log_option = str(g('log_option') or '-l')

    argv = [tool]
    if project_arg_style == 'joined':
        argv.append(f'{project_option}={os.path.normpath(dpa)}')
    else:
        argv += [project_option, os.path.normpath(dpa)]
    if generate_option:
        argv.append(generate_option)
    if gen_type and gen_type_option:
        argv += [gen_type_option, gen_type]
    if modules and modules_option:
        argv += [modules_option, ','.join(modules)]
    if output_option:
        argv += [output_option, os.path.normpath(str(output_dir))]
    if log_option and log_file:
        argv += [log_option, os.path.normpath(log_file)]
    argv += _as_list(g('options'))
    argv += _as_list(getattr(args, 'option', None))

    if tool.lower().endswith(('.cmd', '.bat')):
        argv = ['cmd', '/c', 'call'] + argv
    return argv, dpa, output_dir


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--project', default='Demo_Tc387', help='项目名或 Projects/<name> 路径')
    ap.add_argument('--project-dir', default='', help='显式项目目录 (scons 适配器用)')
    ap.add_argument('--config', default='build.yaml', help='项目 build.yaml 文件名')
    ap.add_argument('--generator', default='', help='codegen.generators 中的名字; 默认第一个 type=vector')
    ap.add_argument('--tool', default='', help='覆盖 DVCfgCmd 路径 (也支持环境变量 DVCFG_CMD)')
    ap.add_argument('--dpa', default='', help='覆盖 .dpa 工程文件路径')
    ap.add_argument('--output', default='', help='覆盖输出目录')
    ap.add_argument('--gen-type', dest='gen_type', default='', help='覆盖生成类型, 如 REAL / VTT')
    ap.add_argument('--module', action='append', default=[], help='只生成指定模块 (可重复)')
    ap.add_argument('--option', action='append', default=[], help='追加原样透传参数 (可重复)')
    ap.add_argument('--dry-run', action='store_true', help='只打印/留痕命令, 不执行 (无 Vector 环境可验证)')
    ap.add_argument('--allow-missing-tool', action='store_true', help='找不到 DVCfgCmd 时不失败')
    ns = ap.parse_args(argv)

    project_dir = Path(ns.project_dir).absolute() if ns.project_dir else _project_dir(ns.project)
    variables = project_variables(project_dir)
    cfg = config_loader.load(str(project_dir / ns.config), variables=variables)
    spec = find_spec(cfg, ns.generator)
    merged, source = merged_config(project_dir, spec, variables)
    cmd, dpa, output_dir = build_command(merged, project_dir, ns)

    quoted = subprocess.list2cmdline([str(a) for a in cmd])
    print(f'[vector] generator : {spec.name} (配置: {source["base"]}'
          + (f' + {source["override"]}' if source['override'] else '') + ')')
    print(f'[vector] dpa       : {dpa}')
    print(f'[vector] output    : {output_dir}')
    print(f'[vector] command   : {quoted}')

    output_dir.mkdir(parents=True, exist_ok=True)
    (output_dir / 'davinci_command.txt').write_text(quoted + '\n', encoding='utf-8')

    tool_to_check = cmd[3] if cmd[:3] == ['cmd', '/c', 'call'] else cmd[0]
    if not _tool_exists(str(tool_to_check)):
        msg = (f'[vector] DVCfgCmd 未找到: {tool_to_check} (Vector 为商业软件, 需本机安装+授权;'
               ' 配置 tool_path 或环境变量 DVCFG_CMD)')
        if ns.dry_run or ns.allow_missing_tool:
            print('[vector] WARN: ' + msg)
            return 0
        print('[vector] ERROR: ' + msg, file=sys.stderr)
        return 2
    if not ns.dry_run and dpa and not os.path.exists(dpa):
        print(f'[vector] ERROR: .dpa 工程不存在: {dpa}', file=sys.stderr)
        return 2

    if ns.dry_run:
        print('[vector] dry-run: 命令未执行 (已写 davinci_command.txt)')
        return 0

    rc = subprocess.call(cmd, cwd=str(ROOT))
    if rc != 0:
        print(f'[vector] 生成失败, rc={rc}', file=sys.stderr)
        return rc
    stamp = output_dir / f'.{spec.name}.stamp'
    stamp.write_text(spec.name + '\n', encoding='utf-8')
    print(f'[vector] done: {stamp}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
