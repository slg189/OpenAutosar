# -*- coding: utf-8 -*-
"""代码生成器抽象基类。"""
import os
import subprocess
from abc import ABC, abstractmethod
from typing import List


class CodegenAdapter(ABC):
    """所有第三方代码生成器的统一接口。"""

    type_name: str = ''

    @abstractmethod
    def build_command(self, spec, output_dir: str) -> List[str]:
        """返回要执行的命令行 argv 列表（已 quote 好）。"""

    @abstractmethod
    def expected_outputs(self, spec, output_dir: str) -> List[str]:
        """返回生成的文件路径列表（供 SCons 注册为 Target）。

        商用工具产物名在生成完成前未知，可以返回一个 .stamp 文件路径。
        """

    # ──────────────────────────────────────────────────────────

    def register(self, env, spec, output_dir: str):
        """把生成器注册成 SCons Builder，返回 Target 节点。"""
        cmd     = self.build_command(spec, output_dir)
        outputs = self.expected_outputs(spec, output_dir)

        # 确保输出目录存在
        for o in outputs:
            os.makedirs(os.path.dirname(o) or '.', exist_ok=True)

        def _run_codegen(target, source, env_):
            for one_cmd in cmd:
                rc = subprocess.call(one_cmd, shell=True)
                if rc != 0:
                    return rc
            stamp = outputs[-1] if outputs[-1].endswith('.stamp') else outputs[0]
            os.makedirs(os.path.dirname(stamp) or '.', exist_ok=True)
            with open(stamp, 'w', encoding='utf-8') as f:
                f.write(spec.name + '\n')
            return 0

        stamp_action = env.Action(_run_codegen, f'[CODEGEN/{spec.type}] {spec.name}')
        target = env.Command(outputs, spec.inputs, stamp_action)

        # 若无显式 inputs（如纯触发型），每次都跑
        if not spec.inputs:
            env.AlwaysBuild(target)
        return target
