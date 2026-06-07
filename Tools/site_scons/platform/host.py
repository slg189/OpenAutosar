# -*- coding: utf-8 -*-
"""主机 (native) 平台 —— 用于环境验证 (host build)。

不注入任何目标 CPU flag, 用主机本地架构编译。配合 toolchain=gcc 验证源码可编译性。
"""


def configure(env):
    env.Append(CPPDEFINES=['HOST_BUILD'])
    env['PLATFORM_CPU']  = 'host'
    env['PLATFORM_ARCH'] = 'native'
    return env
