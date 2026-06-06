# -*- coding: utf-8 -*-
"""Doxygen 调用封装。

需要系统安装 doxygen 命令；Windows 推荐 choco install doxygen.install。
"""
import os
import subprocess
import sys


def run_doxygen(config_template: str, modules: list, bsw_dir: str, output_dir: str) -> int:
    """以模板为基础生成临时 Doxyfile（填入启用模块路径）并执行。"""
    if not os.path.isfile(config_template):
        print(f'[DOXY] 模板不存在：{config_template}', file=sys.stderr)
        return 1

    with open(config_template, encoding='utf-8') as f:
        tpl = f.read()

    input_dirs = []
    for m in modules:
        for sub in ('inc', 'src'):
            p = os.path.join(bsw_dir, m.name, sub)
            if os.path.isdir(p):
                input_dirs.append(p)

    rendered = tpl
    rendered = rendered.replace('@INPUT_DIRS@', ' '.join(f'"{d}"' for d in input_dirs))
    rendered = rendered.replace('@OUTPUT_DIR@', output_dir)
    rendered = rendered.replace('@PROJECT_NAME@', 'OpenAutosar')

    tmp_cfg = os.path.join(output_dir, 'Doxyfile.generated')
    with open(tmp_cfg, 'w', encoding='utf-8') as f:
        f.write(rendered)

    try:
        rc = subprocess.call(['doxygen', tmp_cfg])
    except FileNotFoundError:
        print('[DOXY] 未找到 doxygen 命令，跳过。安装：choco install doxygen.install', file=sys.stderr)
        return 0
    return rc
