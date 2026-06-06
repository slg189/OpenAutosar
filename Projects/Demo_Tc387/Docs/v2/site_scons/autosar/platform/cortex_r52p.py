# -*- coding: utf-8 -*-
"""ARM Cortex-R52+ 平台 —— 骨架。"""


def configure(env):
    cpu_flags = [
        '-mcpu=cortex-r52',
        '-mfloat-abi=hard',
        '-mfpu=neon-fp-armv8',
    ]
    env.Append(CCFLAGS=cpu_flags)
    env.Append(ASFLAGS=cpu_flags)
    env.Append(LINKFLAGS=cpu_flags)
    env['PLATFORM_CPU']  = 'cortex-r52'
    env['PLATFORM_ARCH'] = 'arm'
    return env
