# -*- coding: utf-8 -*-
"""EB tresos AutoCore 适配器。

典型命令行：
    tresos_b.bat -data <workspace> --launcher.suppressErrors
                 -application com.elektrobit.tresos.guide.cli.headless
                 -p <project_name> --generate
"""
import os
from .base import CodegenAdapter


class EbTresosAdapter(CodegenAdapter):
    type_name = 'eb_tresos'

    def build_command(self, spec, output_dir):
        workspace = spec.extra.get('workspace', '')
        project   = spec.extra.get('project',   '')
        return [
            f'"{spec.tool_path}"',
            '-data', f'"{workspace}"',
            '--launcher.suppressErrors',
            '-application', 'com.elektrobit.tresos.guide.cli.headless',
            '-nosplash',
            '-p', f'"{project}"',
            '--generate',
            '-o', f'"{os.path.join(output_dir, "EB")}"',
        ]

    def expected_outputs(self, spec, output_dir):
        stamp = os.path.join(output_dir, 'EB', '.eb.stamp')
        return [stamp]
