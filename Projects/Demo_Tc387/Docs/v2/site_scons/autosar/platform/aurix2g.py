# -*- coding: utf-8 -*-
"""Infineon AURIX 2G (TriCore TC3xx) 平台 —— 骨架。"""


def configure(env):
    # HighTec / TASKING 两套 flag 不同，按当前工具链分支
    tc = env.get('TOOLCHAIN', '').lower()
    if tc == 'hightec':
        env.Append(CCFLAGS=['-mcpu=tc39xx', '-mtc162'])
    elif tc == 'tasking':
        env.Append(CCFLAGS=['--cpu=tc39xx'])

    env.Append(CPPDEFINES=['TRICORE', 'AURIX2G'])
    env['PLATFORM_CPU']  = 'tc39xx'
    env['PLATFORM_ARCH'] = 'tricore'
    return env
