# -*- coding: utf-8 -*-
"""ARM GCC（arm-none-eabi-gcc）工具链。

环境变量：
    ARM_GCC_PREFIX  可选，默认 'arm-none-eabi-'
    ARM_GCC_PATH    可选，工具链 bin/ 目录（不在 PATH 时设置）
"""
import os


def configure(env):
    prefix = os.environ.get('ARM_GCC_PREFIX', 'arm-none-eabi-')
    binpath = os.environ.get('ARM_GCC_PATH', '')
    if binpath:
        env.PrependENVPath('PATH', binpath)

    env['CC']      = prefix + 'gcc'
    env['CXX']     = prefix + 'g++'
    env['AS']      = prefix + 'gcc'        # 用 gcc 驱动汇编以支持 -x assembler-with-cpp
    env['AR']      = prefix + 'ar'
    env['LINK']    = prefix + 'gcc'
    env['RANLIB']  = prefix + 'ranlib'
    env['OBJCOPY'] = prefix + 'objcopy'
    env['OBJDUMP'] = prefix + 'objdump'
    env['SIZE']    = prefix + 'size'

    # 通用 C 标志
    env.Append(CCFLAGS=[
        '-Wall',
        '-Wextra',
        '-Wno-unused-parameter',
        '-ffunction-sections',
        '-fdata-sections',
        '-fno-strict-aliasing',
        '-fno-common',
        '-std=c99',
    ])
    env.Append(ASFLAGS=['-x', 'assembler-with-cpp'])
    env.Append(LINKFLAGS=[
        '-Wl,--gc-sections',
        '-nostartfiles',
    ])

    # Debug / Release
    if env.get('BUILD_TYPE', 'Debug') == 'Release':
        env.Append(CCFLAGS=['-O2'])
        env.Append(CPPDEFINES=['NDEBUG'])
    else:
        env.Append(CCFLAGS=['-O0', '-g3'])
        env.Append(CPPDEFINES=['DEBUG'])

    # 生成器依赖追踪（让 .h 改动能被检测到）
    env.Append(CCFLAGS=['-MMD'])

    return env
