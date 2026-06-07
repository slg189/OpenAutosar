# -*- coding: utf-8 -*-
"""Infineon AURIX 2G (TriCore TC3xx) 平台。

覆盖 TC38x / TC36x 等 TC3xx 衍生 —— 同属 TriCore 1.6.2 (TC162) ISA。
当前随仓库提供的 TriCore 工具链: hightec (GNU tricore-gcc, 用 -mtc162)。
具体衍生 (Tc387/Tc367) 的链接脚本由 build.yaml 的 linker.script 指定。
注: 如需 TASKING 等其它 TriCore 工具链, 新增 toolchain/<name>.py 并在此按 TOOLCHAIN 分支注入
    其 core flag (例: TASKING 用 --core=tc1.6.x)。
"""


def configure(env):
    tc = env.get('TOOLCHAIN', '').lower()

    if tc in ('hightec', 'tricore_gcc', 'gcc'):
        # HighTec GNU TriCore: TC3xx = TriCore 1.6.2 ISA
        cpu = ['-mtc162']
        env.Append(CCFLAGS=cpu)
        env.Append(ASFLAGS=cpu)
        env.Append(LINKFLAGS=cpu)
    else:
        print(f'[WARN] AURIX2G: 未识别工具链 "{tc}", 未注入 CPU flag (按需在 toolchain 层补全)')

    env.Append(CPPDEFINES=['TRICORE', 'AURIX2G', 'TC3XX'])

    env['PLATFORM_CPU']  = 'tc3xx'
    env['PLATFORM_ARCH'] = 'tricore'
    return env
