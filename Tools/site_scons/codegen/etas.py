# -*- coding: utf-8 -*-
"""ETAS ISOLAR-AB / RTA-OS Configurator 适配器。

典型命令行（ISOLAR）：
    ISOLAR.exe -application com.etas.isolar.codegenerator
               -data <workspace> -- -project <name>
"""
import os
from .base import CodegenAdapter


class EtasAdapter(CodegenAdapter):
    type_name = 'etas'

    def build_command(self, spec, output_dir):
        workspace = spec.extra.get('workspace', '')
        project   = spec.extra.get('project',   '')
        return [[
            spec.tool_path,
            '-application', 'com.etas.isolar.codegenerator',
            '-data', workspace,
            '-nosplash',
            '--',
            '-project', project,
            '-output',  os.path.join(output_dir, 'Etas'),
        ]]

    def expected_outputs(self, spec, output_dir):
        stamp = os.path.join(output_dir, 'Etas', '.etas.stamp')
        return [stamp]
