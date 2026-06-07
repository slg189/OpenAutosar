# -*- coding: utf-8 -*-
"""ARM Cortex-M33（带 FPU + DSP）平台。"""


def configure(env):
    cpu_flags = [
        '-mcpu=cortex-m33',
        '-mthumb',
        '-mfloat-abi=hard',
        '-mfpu=fpv5-sp-d16',
    ]
    env.Append(CCFLAGS=cpu_flags)
    env.Append(ASFLAGS=cpu_flags)
    env.Append(LINKFLAGS=cpu_flags)

    env.Append(CPPDEFINES=[
        'ARM_MATH_CM33',
        '__FPU_PRESENT=1',
    ])

    env['PLATFORM_CPU']  = 'cortex-m33'
    env['PLATFORM_ARCH'] = 'arm'
    return env
