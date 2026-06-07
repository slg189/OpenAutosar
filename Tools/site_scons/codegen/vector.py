# -*- coding: utf-8 -*-
"""Vector DaVinci Configurator Pro / GENy 适配器。

典型命令行（DaVinci Configurator Pro）：
    DVCfgCmd.exe -c project.dpa -g output_dir [-l log.txt]
"""
import glob
import os
from .base import CodegenAdapter


class VectorAdapter(CodegenAdapter):
    type_name = 'vector'

    def build_command(self, spec, output_dir):
        if not spec.inputs:
            raise ValueError('Vector 生成器至少需要一个 input (.dpa)')
        input_file = spec.inputs[0]
        out = os.path.join(output_dir, 'Vector').replace('\\', '/')
        log = os.path.join(out, 'codegen.log').replace('\\', '/')

        return [[
            spec.tool_path,
            '-c', input_file,
            '-g', out,
            '-l', log,
        ]]

    def expected_outputs(self, spec, output_dir):
        out = os.path.join(output_dir, 'Vector')
        # 真实文件名未知，先扫描已有；最后 stamp 文件用于强制依赖
        files = []
        if spec.outputs_glob:
            files = glob.glob(spec.outputs_glob, recursive=True)
        stamp = os.path.join(out, '.vector.stamp')
        return files + [stamp]
