# -*- coding: utf-8 -*-
"""scons list / --explain 输出 (把自动推断显式化)。"""
import os


def _rel(root, p):
    try:
        return os.path.relpath(p, root)
    except ValueError:
        return p


def print_list(root, decisions, undeclared):
    """打印解析后的构建计划 + 工作区中未声明的模块。"""
    print('\n================ 构建计划 (scons list) ================')
    print('%-6s %-22s %-7s %s' % ('LAYER', 'MODULE', 'MODE', 'SOURCE/LIB'))
    print('-' * 70)
    for d in decisions:
        if d.kind == 'source':
            where = _rel(root, d.src_dir) + '/src'
        elif d.kind == 'lib':
            where = _rel(root, d.a_file) + (' [release->*_Libs]' if d.lib_from_module else '')
        elif d.kind == 'header':
            where = _rel(root, d.src_dir) + '/inc (仅头文件)'
        else:
            where = 'ERROR: ' + d.reason
        print('%-6s %-22s %-7s %s' % (d.layer, d.modname, d.kind, where))
    if undeclared:
        print('\n[提示] 工作区中存在、但 build.yaml 未声明的模块 (可加入 layers):')
        for layer, mods in undeclared.items():
            print('  %-6s %s' % (layer, ', '.join(mods)))
    print('=' * 56 + '\n')


def print_explain(root, decision):
    """解释单个模块为何 source/lib。"""
    d = decision
    print('\n---------------- scons --explain %s ----------------' % d.modname)
    print('  层      : %s' % d.layer)
    print('  模块    : %s  (leaf=%s)' % (d.modname, d.leaf))
    print('  判定    : %s' % d.kind)
    print('  原因    : %s' % d.reason)
    if d.kind == 'source':
        print('  源码目录: %s' % _rel(root, d.src_dir))
    elif d.kind == 'lib':
        print('  链接库  : %s' % _rel(root, d.a_file))
        print('  头文件  : %s' % ', '.join(_rel(root, i) for i in d.inc_dirs))
        if d.lib_from_module:
            print('  发布    : release 时 .a + inc 更新到 %s'
                  % _rel(root, os.path.join(root, '%s_Libs' % d.layer, d.modname)))
    elif d.kind == 'header':
        print('  头文件  : %s  (仅头文件, 不编译不链接)'
              % ', '.join(_rel(root, i) for i in d.inc_dirs))
    if d.tried:
        print('  尝试过  : %s' % ', '.join(_rel(root, t) for t in d.tried))
    print('-' * 56 + '\n')
