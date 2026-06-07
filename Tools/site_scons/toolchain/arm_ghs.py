# -*- coding: utf-8 -*-
"""Green Hills Software (GHS) ARM 工具链 —— 骨架。

TODO: 根据实际安装路径与 license 配置补全。
参考：原 Tools/Build/Profile/arm_ghs/*.mak（加密，需要反向对照实际命令行）。
"""
import os


def configure(env):
    bin_dir = os.environ.get('GHS_ARM_BIN', '')
    if bin_dir:
        env.PrependENVPath('PATH', bin_dir)

    env['CC']      = 'ccarm'
    env['CXX']     = 'cxarm'
    env['AS']      = 'asarm'
    env['AR']      = 'ax'
    env['LINK']    = 'elxr'
    env['OBJCOPY'] = 'gstrip'        # 或 elfconv，按 GHS 版本

    # TODO: 补全 -bsp/-cpu/-fpu 等 GHS 特有参数
    env.Append(CCFLAGS=[
        '--no_commons',
        # '-bspxxx', '-cpuxxx',
    ])
    env.Append(LINKFLAGS=[
        # '-T linker.ld',  by platform layer
    ])

    if env.get('BUILD_TYPE', 'Debug') == 'Release':
        env.Append(CCFLAGS=['-Ospeed'])
    else:
        env.Append(CCFLAGS=['-G', '-Onone'])

    return env
