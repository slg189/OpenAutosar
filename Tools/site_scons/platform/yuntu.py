# -*- coding: utf-8 -*-
"""SemiDrive YunTu (含 RISC-V SA32D610 + ARM Cortex-A7) 平台 —— 骨架。"""


def configure(env):
    # RISC-V 还是 ARM 由具体 core 决定；按需在 build.yaml 加 core 字段
    core = env.get('PLATFORM_CORE', 'arm').lower()
    if core == 'riscv':
        env.Append(CCFLAGS=['-march=rv32imac', '-mabi=ilp32'])
        env['PLATFORM_ARCH'] = 'riscv'
    else:
        env.Append(CCFLAGS=['-mcpu=cortex-a7', '-mfloat-abi=hard', '-mfpu=neon'])
        env['PLATFORM_ARCH'] = 'arm'

    env.Append(CPPDEFINES=['SEMIDRIVE_YUNTU'])
    return env
