# -*- coding: utf-8 -*-
"""Infineon AURIX 2G (TriCore TC3xx) 平台。

覆盖 TC38x / TC36x 等 TC3xx 衍生 —— 同属 TriCore 1.6.2 (TC162) ISA。
按当前工具链注入 core flag:
    hightec  : GNU TriCore (tricore-gcc), 用 -mtc162
    tasking  : TASKING ctc/ltc, 用 --core=tc1.6.x  (⚠ tasking 适配为骨架, 按需补全)
具体衍生 (Tc387/Tc367) 的链接脚本由 build.yaml 的 linker.script 指定。
"""


def configure(env):
    tc = env.get('TOOLCHAIN', '').lower()

    if tc in ('hightec', 'tricore_gcc', 'gcc'):
        # HighTec GNU TriCore: TC3xx = TriCore 1.6.2 ISA
        cpu = ['-mtc162']
        env.Append(CCFLAGS=cpu)
        env.Append(ASFLAGS=cpu)
        env.Append(LINKFLAGS=cpu)
    elif tc == 'tasking':
        # TASKING VX-toolset for TriCore (骨架)
        env.Append(CCFLAGS=['--core=tc1.6.x'])
        env.Append(ASFLAGS=['--core=tc1.6.x'])
        env.Append(LINKFLAGS=['--core=tc1.6.x'])
    else:
        print(f'[WARN] AURIX2G: 未识别工具链 "{tc}", 未注入 CPU flag (按需在 toolchain 层补全)')

    env.Append(CPPDEFINES=['TRICORE', 'AURIX2G', 'TC3XX'])

    env['PLATFORM_CPU']  = 'tc3xx'
    env['PLATFORM_ARCH'] = 'tricore'
    return env
