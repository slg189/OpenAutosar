# -*- coding: utf-8 -*-
"""ETAS ISOLAR-B / RTA-BSW code generation adapter.

Typical ISOLAR-B headless generation uses options such as:
    ISOLAR-B.cmd -generate -p=<project> -t=<target>

Workspace, output directory and version-specific options are delegated to
Tools/scripts/run_etas_isolar_b.py so the same command can be validated
manually with --dry-run.
"""
import os
import sys

from .base import CodegenAdapter


class EtasAdapter(CodegenAdapter):
    type_name = 'etas'

    def build_command(self, spec, output_dir):
        project_dir = os.path.dirname(os.path.abspath(output_dir))
        return [[
            sys.executable,
            os.path.join('Tools', 'scripts', 'run_etas_isolar_b.py'),
            '--project-dir', project_dir,
            '--generator', spec.name,
        ]]

    def expected_outputs(self, spec, output_dir):
        out = spec.extra.get('output_dir') or spec.extra.get('output') or os.path.join(output_dir, 'BSW')
        stamp = os.path.join(out, f'.{spec.name}.stamp')
        return [stamp]
