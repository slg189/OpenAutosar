# -*- coding: utf-8 -*-
"""IAR for ARM 工具链 —— 骨架。

TODO: 补全 IAR Embedded Workbench 命令行参数。
"""
import os


def configure(env):
    bin_dir = os.environ.get('IAR_ARM_BIN', '')
    if bin_dir:
        env.PrependENVPath('PATH', bin_dir)

    env['CC']   = 'iccarm'
    env['CXX']  = 'iccarm'
    env['AS']   = 'iasmarm'
    env['AR']   = 'iarchive'
    env['LINK'] = 'ilinkarm'

    env.Append(CCFLAGS=[
        '--silent',
        '-e',                   # 启用语言扩展
        # '--cpu=Cortex-M33',   by platform layer
    ])
    env.Append(LINKFLAGS=[
        '--silent',
        # '--config linker.icf',  by platform layer
    ])

    if env.get('BUILD_TYPE', 'Debug') == 'Release':
        env.Append(CCFLAGS=['-Ohs'])
    else:
        env.Append(CCFLAGS=['-On', '--debug'])

    return env
