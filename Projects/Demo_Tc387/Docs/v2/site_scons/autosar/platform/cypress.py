# -*- coding: utf-8 -*-
"""Cypress / Infineon Traveo II (TVII) 平台 — 基于 Cortex-M7 双核。"""


def configure(env):
    cpu_flags = [
        '-mcpu=cortex-m7',
        '-mthumb',
        '-mfloat-abi=hard',
        '-mfpu=fpv5-d16',
    ]
    env.Append(CCFLAGS=cpu_flags)
    env.Append(ASFLAGS=cpu_flags)
    env.Append(LINKFLAGS=cpu_flags)

    env.Append(CPPDEFINES=[
        'CY_TVII',
        'CYT4BF',
        '__FPU_PRESENT=1',
    ])

    env['PLATFORM_CPU']  = 'cortex-m7'
    env['PLATFORM_ARCH'] = 'arm'
    return env
