# -*- coding: utf-8 -*-
"""模块依赖图生成。"""
import glob
import os
import re

_INCLUDE_RE = re.compile(r'^\s*#\s*include\s+["<]([^">]+)[">]', re.MULTILINE)


def infer_dependencies(bsw_dir: str, modules: list) -> dict:
    """基于 #include 扫描推断模块间依赖。返回 {mod: [dep1, dep2]}。"""
    deps = {m: set() for m in modules}
    mod_set = set(modules)

    for m in modules:
        for src in glob.glob(os.path.join(bsw_dir, m, 'src', '*.c')):
            try:
                with open(src, encoding='utf-8', errors='ignore') as f:
                    text = f.read()
            except OSError:
                continue
            for inc in _INCLUDE_RE.findall(text):
                # 假设头文件名前缀就是模块名：CanIf.h → CanIf, Com_Cbk.h → Com
                guess = inc.split('.')[0].split('_')[0]
                if guess in mod_set and guess != m:
                    deps[m].add(guess)
    return {k: sorted(v) for k, v in deps.items()}


def render_mermaid(modules: list, deps: dict) -> str:
    """生成 Mermaid graph TD 文本。

    Args:
        modules: [{'name': 'CanIf', 'enabled': True}]
        deps:    {'CanIf': ['Can', 'PduR']}
    """
    lines = ['```mermaid', 'graph TD']
    for m in modules:
        marker = '' if m['enabled'] else ':::disabled'
        lines.append(f'  {m["name"]}{marker}')
    for src, targets in deps.items():
        for t in targets:
            lines.append(f'  {src} --> {t}')
    lines.append('  classDef disabled fill:#eee,stroke:#999,color:#999')
    lines.append('```')
    return '\n'.join(lines)


def render_graphviz(modules: list, deps: dict) -> str:
    """生成 .dot 文件文本，供 graphviz 渲染成 svg。"""
    lines = ['digraph G {', '  rankdir=LR;', '  node [shape=box, style=rounded];']
    for m in modules:
        attr = '' if m['enabled'] else ' [style="filled,rounded", fillcolor="#eeeeee"]'
        lines.append(f'  "{m["name"]}"{attr};')
    for src, targets in deps.items():
        for t in targets:
            lines.append(f'  "{src}" -> "{t}";')
    lines.append('}')
    return '\n'.join(lines)
