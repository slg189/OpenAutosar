#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Run ETAS ISOLAR-B command-line code generation for AUTOSAR BSW.

The script reads the project build.yaml `codegen.generators` entry with
`type: etas`, builds the ISOLAR-B.cmd command, and optionally executes it.
Use --dry-run to validate the final command on machines without ETAS installed.
"""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


_CWD = Path.cwd()
if (_CWD / 'Tools' / 'scripts' / 'run_etas_isolar_b.py').exists():
    ROOT = _CWD
else:
    ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'Tools'))
venv_site = ROOT / 'Tools' / '.venv' / 'Lib' / 'site-packages'
if venv_site.is_dir():
    sys.path.insert(0, str(venv_site))

from site_scons import config_loader  # noqa: E402


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


def _find_spec(cfg, name: str):
    etas = [s for s in cfg.codegen_specs if s.type == 'etas']
    if name:
        for spec in etas:
            if spec.name == name:
                return spec
        raise SystemExit(f'[etas] generator not found: {name}')
    if not etas:
        raise SystemExit('[etas] no codegen generator with type: etas in build.yaml')
    return etas[0]


def _quote_cmd(argv):
    return subprocess.list2cmdline([str(a) for a in argv])


def _native_path(value):
    text = str(value)
    if not text:
        return text
    if any(sep in text for sep in ('\\', '/')) or ':' in text:
        return os.path.normpath(text)
    return text


def build_command(spec, project_dir: Path, cfg_output_dir: str, args):
    extra = spec.extra or {}

    tool = (
        args.tool
        or spec.tool_path
        or os.environ.get('ISOLAR_B_CMD')
        or 'ISOLAR-B.cmd'
    )
    workspace = Path(_native_path(args.workspace or extra.get('workspace') or project_dir / 'Cfg' / 'BSW'))
    output_dir = Path(_native_path(args.output or extra.get('output_dir') or extra.get('output') or Path(cfg_output_dir) / 'BSW'))
    isolar_project = _native_path(args.isolar_project or extra.get('isolar_project') or extra.get('project') or project_dir / 'Cfg' / 'BSW')
    target = args.target or extra.get('target') or extra.get('generation_target') or 'RTA-BSW'

    nosplash = bool(extra.get('nosplash', True))
    workspace_option = str(extra.get('workspace_option', '-data'))
    generate_option = str(extra.get('generate_option', '-generate'))
    project_option = str(extra.get('project_option', '-p'))
    target_option = str(extra.get('target_option', '-t'))
    output_option = str(extra.get('output_option', '-o'))
    project_arg_style = str(extra.get('project_arg_style', 'joined'))  # joined: -p=Name, split: -p Name
    target_arg_style = str(extra.get('target_arg_style', 'joined'))

    argv = [str(tool)]
    if nosplash:
        argv.append('-nosplash')
    if workspace_option:
        argv += [workspace_option, _native_path(workspace)]
    if generate_option:
        argv.append(generate_option)

    if project_arg_style == 'split':
            argv += [project_option, _native_path(isolar_project)]
    else:
        argv.append(f'{project_option}={_native_path(isolar_project)}')

    if target:
        if target_arg_style == 'split':
            argv += [target_option, str(target)]
        else:
            argv.append(f'{target_option}={target}')

    if output_option:
        argv += [output_option, _native_path(output_dir)]

    argv += _as_list(extra.get('options'))
    argv += _as_list(extra.get('extra_options'))
    argv += _as_list(args.option)

    if str(tool).lower().endswith(('.cmd', '.bat')):
        argv = ['cmd', '/c', 'call'] + argv

    return argv, workspace, output_dir, target


def _tool_exists(tool_arg: str) -> bool:
    if tool_arg.lower() in ('cmd', 'cmd.exe'):
        return True
    p = Path(tool_arg)
    if p.is_absolute() or any(sep in tool_arg for sep in ('\\', '/')):
        return p.exists()
    return shutil.which(tool_arg) is not None


def main(argv=None):
    ap = argparse.ArgumentParser(description='Run ETAS ISOLAR-B BSW code generation.')
    ap.add_argument('--project', default='Demo_Tc387', help='Project name or Projects/<name> path.')
    ap.add_argument('--project-dir', default='', help='Explicit project directory path.')
    ap.add_argument('--config', default='build.yaml', help='Project build.yaml file name.')
    ap.add_argument('--generator', default='', help='codegen.generators name; defaults to first type=etas.')
    ap.add_argument('--tool', default='', help='Override ISOLAR-B.cmd path. Also supports ISOLAR_B_CMD env var.')
    ap.add_argument('--workspace', default='', help='Override ISOLAR workspace/config dir; defaults to Projects/<P>/Cfg/BSW.')
    ap.add_argument('--isolar-project', default='', help='Override ISOLAR project name/path passed to -p.')
    ap.add_argument('--target', default='', help='Override generation target passed to -t, e.g. RTA-BSW.')
    ap.add_argument('--output', default='', help='Override generated output dir; defaults to Projects/<P>/Gen/BSW.')
    ap.add_argument('--option', action='append', default=[], help='Append raw option; can be repeated.')
    ap.add_argument('--dry-run', action='store_true', help='Print command and validate paths without running ISOLAR-B.')
    ap.add_argument('--allow-missing-tool', action='store_true', help='Do not fail when ISOLAR-B.cmd cannot be found.')
    ns = ap.parse_args(argv)

    project_dir = Path(ns.project_dir).absolute() if ns.project_dir else _project_dir(ns.project)
    yaml_path = project_dir / ns.config
    variables = {
        'ROOT_DIR': str(ROOT),
        'PROJECT_DIR': str(project_dir),
        'BSW_DIR': str(ROOT / 'BSW'),
        'MCAL_DIR': str(ROOT / 'MCAL'),
        'COMP_DIR': str(ROOT / 'Components'),
        'GEN_DIR': str(project_dir / 'Gen'),
    }
    cfg = config_loader.load(str(yaml_path), variables=variables)
    spec = _find_spec(cfg, ns.generator)
    cmd, workspace, output_dir, target = build_command(spec, project_dir, cfg.codegen_output_dir, ns)

    print(f'[etas] generator : {spec.name}')
    print(f'[etas] workspace : {workspace}')
    print(f'[etas] output    : {output_dir}')
    print(f'[etas] target    : {target}')
    print(f'[etas] command   : {_quote_cmd(cmd)}')

    output_dir.mkdir(parents=True, exist_ok=True)
    command_log = output_dir / 'isolar_b_command.txt'
    command_log.write_text(_quote_cmd(cmd) + '\n', encoding='utf-8')

    tool_to_check = cmd[3] if cmd[:3] == ['cmd', '/c', 'call'] else cmd[0]
    if not _tool_exists(str(tool_to_check)):
        msg = f'[etas] ISOLAR-B tool not found: {tool_to_check}'
        if ns.dry_run or ns.allow_missing_tool:
            print('[etas] WARN: ' + msg)
            return 0
        print('[etas] ERROR: ' + msg, file=sys.stderr)
        print('[etas] Set codegen.generators[].tool_path or ISOLAR_B_CMD.', file=sys.stderr)
        return 2

    if ns.dry_run:
        print('[etas] dry-run: command not executed')
        return 0

    rc = subprocess.call(cmd, cwd=str(ROOT))
    if rc != 0:
        return rc

    stamp = output_dir / f'.{spec.name}.stamp'
    stamp.write_text(spec.name + '\n', encoding='utf-8')
    print(f'[etas] done: {stamp}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
