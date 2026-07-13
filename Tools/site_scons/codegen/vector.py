# -*- coding: utf-8 -*-
"""Vector DaVinci Configurator 适配器。

典型命令行 (DaVinci Configurator, 选项名随版本可在 generator extra 里调):
    DVCfgCmd.exe -p <project.dpa> -g [--genType REAL] [-l log]

命令拼装/配置分层 (build.yaml <- vector_codegen.json <- CLI) 委托给
Tools/scripts/run_vector_davinci.py, 同一条命令可用 --dry-run 手工验证。
"""
import json
import os
import sys

from .base import CodegenAdapter


class VectorAdapter(CodegenAdapter):
    type_name = 'vector'

    def build_command(self, spec, output_dir):
        project_dir = os.path.dirname(os.path.abspath(output_dir))
        return [[
            sys.executable,
            os.path.join('Tools', 'scripts', 'run_vector_davinci.py'),
            '--project-dir', project_dir,
            '--generator', spec.name,
        ]]

    def expected_outputs(self, spec, output_dir):
        project_dir = os.path.dirname(os.path.abspath(output_dir))
        out = spec.extra.get('output_dir') or os.path.join(output_dir, 'Vector')
        ov = os.path.join(project_dir, 'vector_codegen.json')  # 页面覆盖层可能改了输出目录
        if os.path.exists(ov):
            try:
                with open(ov, encoding='utf-8') as fh:
                    out = json.load(fh).get('output_dir') or out
            except (OSError, ValueError):
                pass
        stamp = os.path.join(out, f'.{spec.name}.stamp')
        return [stamp]
