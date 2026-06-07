# -*- coding: utf-8 -*-
"""ARM Cortex-R5（带 FPU）平台 —— 骨架。"""


def configure(env):
    cpu_flags = [
        '-mcpu=cortex-r5',
        '-mfloat-abi=hard',
        '-mfpu=vfpv3-d16',
        # '-mbig-endian',     # 如果是大端字节序
    ]
    env.Append(CCFLAGS=cpu_flags)
    env.Append(ASFLAGS=cpu_flags)
    env.Append(LINKFLAGS=cpu_flags)
    env['PLATFORM_CPU']  = 'cortex-r5'
    env['PLATFORM_ARCH'] = 'arm'
    return env
