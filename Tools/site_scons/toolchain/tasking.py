# -*- coding: utf-8 -*-
"""TASKING for TriCore 工具链 —— 骨架。

TODO: 补全 ctc/ltc/astc 命令行参数。
"""
import os


def configure(env):
    bin_dir = os.environ.get('TASKING_BIN', '')
    if bin_dir:
        env.PrependENVPath('PATH', bin_dir)

    env['CC']   = 'ctc'
    env['AS']   = 'astc'
    env['AR']   = 'artc'
    env['LINK'] = 'ltc'

    env.Append(CCFLAGS=[
        # '--cpu=tc39xx',  by platform layer
        '--iso=99',
        '--language=+volatile',
    ])
    env.Append(LINKFLAGS=[
        # '-d linker.lsl',  by platform layer
    ])

    if env.get('BUILD_TYPE', 'Debug') == 'Release':
        env.Append(CCFLAGS=['-O3'])
    else:
        env.Append(CCFLAGS=['-O0', '-g'])

    return env
