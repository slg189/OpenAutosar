# -*- coding: utf-8 -*-
"""自研/通用 shell 命令适配器。

YAML 中写：
    type: custom
    cmd:  "python ${ROOT_DIR}/Tools/rte_gen.py -i ${INPUT} -o ${OUTPUT_DIR}/Rte"
    inputs:
      - ${PROJECT_DIR}/Cfg/.../SystemExtract.arxml

支持的占位符在 cmd 中：
    ${INPUT}      → spec.inputs[0]
    ${OUTPUT_DIR} → output_dir 参数
"""
import os
import shlex
from .base import CodegenAdapter


class CustomAdapter(CodegenAdapter):
    type_name = 'custom'

    def build_command(self, spec, output_dir):
        cmd_template = spec.extra.get('cmd', '')
        if not cmd_template:
            raise ValueError('custom 适配器需要 extra.cmd 字段')

        cmd = cmd_template
        cmd = cmd.replace('${OUTPUT_DIR}', output_dir)
        if spec.inputs:
            cmd = cmd.replace('${INPUT}', spec.inputs[0])

        # 返回 shell 一行（不拆 argv，保留用户引号风格）
        return [cmd]

    def expected_outputs(self, spec, output_dir):
        stamp = os.path.join(output_dir, f'.{spec.name}.stamp')
        return [stamp]
