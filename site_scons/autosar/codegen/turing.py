# -*- coding: utf-8 -*-
"""图灵 TuringAUTOSAR 适配器。

典型命令行：
    TuringGen.exe -c <config.json> -o <output_dir>
"""
import os
from .base import CodegenAdapter


class TuringAdapter(CodegenAdapter):
    type_name = 'turing'

    def build_command(self, spec, output_dir):
        config = spec.extra.get('config', '')
        if not config:
            raise ValueError('turing 适配器需要 extra.config 字段')
        return [
            f'"{spec.tool_path}"',
            '-c', f'"{config}"',
            '-o', f'"{os.path.join(output_dir, "Turing")}"',
        ]

    def expected_outputs(self, spec, output_dir):
        stamp = os.path.join(output_dir, 'Turing', '.turing.stamp')
        return [stamp]
