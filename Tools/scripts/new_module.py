#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""为指定 BSW 模块自动生成 SConscript 脚手架。

用法：
    python scripts/new_module.py --bsw CanIf
    python scripts/new_module.py --bsw Com Os Crc
    python scripts/new_module.py --mcal Cypress/Mcu
"""
import argparse
import os
import sys


SCRIPT_TEMPLATE = '''# -*- coding: utf-8 -*-
"""SConscript for {kind}/{name} —— 自动生成于 new_module.py。"""
Import('env')
try:
    Import('mod_cfg')
except Exception:
    mod_cfg = None

from site_scons.module_builder import build_module

objs = build_module(env, Dir('.').abspath, '{name}', mod_cfg)
Return('objs')
'''


def make_one(root: str, kind: str, name: str, force: bool = False) -> int:
    base = {
        'bsw':  os.path.join(root, 'BSW'),
        'mcal': os.path.join(root, 'MCAL'),
        'comp': os.path.join(root, 'Components'),
    }[kind]
    mod_dir = os.path.join(base, name)
    if not os.path.isdir(mod_dir):
        print(f'[ERR ] 模块目录不存在：{mod_dir}', file=sys.stderr)
        return 1

    scons_file = os.path.join(mod_dir, 'SConscript')
    if os.path.exists(scons_file) and not force:
        print(f'[SKIP] 已存在：{scons_file}（用 --force 覆盖）')
        return 0

    leaf = os.path.basename(name)        # MCAL/Cypress/Mcu → Mcu
    content = SCRIPT_TEMPLATE.format(kind={'bsw':'BSW','mcal':'MCAL','comp':'Components'}[kind],
                                     name=leaf)
    with open(scons_file, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f'[OK  ] 已生成：{scons_file}')
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default=os.getcwd(), help='项目根目录')
    ap.add_argument('--bsw',  nargs='*', default=[], help='BSW 模块名列表')
    ap.add_argument('--mcal', nargs='*', default=[], help='MCAL 模块路径列表')
    ap.add_argument('--comp', nargs='*', default=[], help='Component 模块名列表')
    ap.add_argument('--force', action='store_true', help='覆盖已存在的 SConscript')
    args = ap.parse_args()

    rc = 0
    for n in args.bsw:  rc |= make_one(args.root, 'bsw',  n, args.force)
    for n in args.mcal: rc |= make_one(args.root, 'mcal', n, args.force)
    for n in args.comp: rc |= make_one(args.root, 'comp', n, args.force)
    return rc


if __name__ == '__main__':
    sys.exit(main())
