# -*- coding: utf-8 -*-
"""扫描模块 inc/*.h，抽取函数原型 + Doxygen 注释。"""
import glob
import os
import pathlib
import re

# /** ... */  void Foo(int a);  形式
_FUNC_RE = re.compile(
    r'/\*\*\s*(?P<doc>.*?)\*/\s*'
    r'(?P<sig>[\w\s\*\(\)\,]+?\s+\w+\s*\([^;]*?\))\s*;',
    re.DOTALL,
)

# 没有 /** */ 的裸函数原型
_BARE_FUNC_RE = re.compile(
    r'^\s*(extern\s+)?'
    r'(?P<sig>(?:void|int|uint8|uint16|uint32|Std_ReturnType|[A-Z]\w+_(?:ReturnType|StatusType))'
    r'\s+\w+\s*\([^;]*?\))\s*;',
    re.MULTILINE,
)


def extract_module_interfaces(module_dir: str) -> list:
    """返回 [{file, signature, doc}] 列表。"""
    results = []
    inc_dir = os.path.join(module_dir, 'inc')
    if not os.path.isdir(inc_dir):
        return results

    for header in sorted(glob.glob(os.path.join(inc_dir, '*.h'))):
        try:
            text = pathlib.Path(header).read_text(encoding='utf-8', errors='ignore')
        except OSError:
            continue
        seen = set()

        # 1. 优先带 doxygen 注释的
        for m in _FUNC_RE.finditer(text):
            sig = ' '.join(m.group('sig').split())
            seen.add(sig)
            results.append({
                'file':      os.path.basename(header),
                'signature': sig,
                'doc':       m.group('doc').strip(),
            })

        # 2. 补抓裸原型
        for m in _BARE_FUNC_RE.finditer(text):
            sig = ' '.join(m.group('sig').split())
            if sig in seen:
                continue
            results.append({
                'file':      os.path.basename(header),
                'signature': sig,
                'doc':       '',
            })

    return results


def render_markdown(module_name: str, ifaces: list) -> str:
    lines = [f'# {module_name} 接口清单\n',
             f'> 共 {len(ifaces)} 个公开接口，自动生成于构建系统。\n']
    by_file = {}
    for i in ifaces:
        by_file.setdefault(i['file'], []).append(i)
    for fname, lst in sorted(by_file.items()):
        lines.append(f'\n## `{fname}`\n')
        for i in lst:
            lines.append(f'### `{i["signature"]}`\n')
            if i['doc']:
                lines.append(i['doc'] + '\n')
            else:
                lines.append('_（无文档注释）_\n')
    return '\n'.join(lines)
