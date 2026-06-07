#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""为指定层创建一个模块目录骨架 (inc/ + src/ + 桩 .h/.c)。

v3 构建按"约定自动发现"模块 (目录含 inc/ 或 src/ 即视为模块), 不再需要每模块 SConscript。
本脚本只生成目录骨架; 之后可在 Projects/<P>/build.yaml 的 layers 下显式声明 (可选, 用于
指定 mode/cflags 等), 否则也会被自动发现。

用法:
    python Tools/scripts/new_module.py --bsw CanIf
    python Tools/scripts/new_module.py --bsw Com Os --asw Diagnostics
    python Tools/scripts/new_module.py --mcal Tc387/Eth        # MCAL 带芯片
    python Tools/scripts/new_module.py --comp Mcu/Wdg
"""
import argparse
import os
import sys

LAYER_BASE = {'bsw': 'BSW', 'mcal': 'MCAL', 'cdd': 'CDD', 'asw': 'ASW', 'comp': 'Components'}

HEADER_TMPL = '''/**
 * @file    {leaf}.h
 * @brief   {leaf} 模块对外接口 (骨架, 由 new_module.py 生成)。
 */
#ifndef {guard}
#define {guard}

#include "Std_Types.h"

/* TODO: 声明 {leaf} 的对外接口 */
void {leaf}_Init(void);

#endif /* {guard} */
'''

SOURCE_TMPL = '''/**
 * @file    {leaf}.c
 * @brief   {leaf} 模块实现 (骨架, 由 new_module.py 生成)。
 */
#include "{leaf}.h"

void {leaf}_Init(void)
{{
    /* TODO: 实现 {leaf} 初始化 */
}}
'''


def make_one(root, layer, name, force=False):
    base = LAYER_BASE[layer]
    mod_dir = os.path.join(root, base, name)
    leaf = os.path.basename(name)
    inc_dir = os.path.join(mod_dir, 'inc')
    src_dir = os.path.join(mod_dir, 'src')
    h = os.path.join(inc_dir, leaf + '.h')
    c = os.path.join(src_dir, leaf + '.c')

    if (os.path.exists(h) or os.path.exists(c)) and not force:
        print(f'[SKIP] 已存在: {os.path.relpath(mod_dir, root)} (用 --force 覆盖)')
        return 0

    os.makedirs(inc_dir, exist_ok=True)
    os.makedirs(src_dir, exist_ok=True)
    with open(h, 'w', encoding='utf-8') as f:
        f.write(HEADER_TMPL.format(leaf=leaf, guard=leaf.upper() + '_H'))
    with open(c, 'w', encoding='utf-8') as f:
        f.write(SOURCE_TMPL.format(leaf=leaf))

    rel = os.path.relpath(mod_dir, root).replace('\\', '/')
    print(f'[OK  ] {rel}/ (inc/{leaf}.h + src/{leaf}.c)')
    print(f'        → 自动发现即可编译; 如需显式控制, 在 build.yaml 的 layers.{base} 下加:')
    print(f'              {leaf}: {{ mode: source }}')
    return 0


def main():
    ap = argparse.ArgumentParser(description='生成模块目录骨架 (v3 自动发现)')
    ap.add_argument('--root', default=os.getcwd(), help='仓库根 (默认当前目录)')
    ap.add_argument('--bsw',  nargs='*', default=[], help='BSW 模块名')
    ap.add_argument('--mcal', nargs='*', default=[], help='MCAL 模块名 (可含芯片, 如 Tc387/Eth)')
    ap.add_argument('--cdd',  nargs='*', default=[], help='CDD 模块名')
    ap.add_argument('--asw',  nargs='*', default=[], help='ASW 模块名')
    ap.add_argument('--comp', nargs='*', default=[], help='Components 模块名 (可嵌套, 如 Mcu/Wdg)')
    ap.add_argument('--force', action='store_true', help='覆盖已存在文件')
    args = ap.parse_args()

    layers = ('bsw', 'mcal', 'cdd', 'asw', 'comp')
    if not any(getattr(args, l) for l in layers):
        ap.print_help()
        return 0
    rc = 0
    for layer in layers:
        for name in getattr(args, layer):
            rc |= make_one(args.root, layer, name, args.force)
    return rc


if __name__ == '__main__':
    sys.exit(main())
