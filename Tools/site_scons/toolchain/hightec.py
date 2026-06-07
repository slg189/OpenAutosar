# -*- coding: utf-8 -*-
"""HighTec GNU TriCore 工具链 —— 骨架。

TODO: 补全 tricore-gcc 命令行参数。
"""
import os


def configure(env):
    bin_dir = os.environ.get('HIGHTEC_BIN', '')
    if bin_dir:
        env.PrependENVPath('PATH', bin_dir)

    env['CC']      = 'tricore-gcc'
    env['CXX']     = 'tricore-g++'
    env['AS']      = 'tricore-gcc'
    env['AR']      = 'tricore-ar'
    env['LINK']    = 'tricore-gcc'
    env['OBJCOPY'] = 'tricore-objcopy'
    env['SIZE']    = 'tricore-size'

    env.Append(CCFLAGS=[
        '-Wall',
        '-ffunction-sections',
        '-fdata-sections',
        # '-mcpu=tc39xx',  by platform layer
    ])
    # 注: map 文件由 build.yaml 的 output.formats 控制 (项目 SConscript 注入), 此处不写死
    env.Append(LINKFLAGS=[
        '-Wl,--gc-sections',
    ])

    if env.get('BUILD_TYPE', 'Debug') == 'Release':
        env.Append(CCFLAGS=['-O2'])
    else:
        env.Append(CCFLAGS=['-O0', '-g3'])

    return env
