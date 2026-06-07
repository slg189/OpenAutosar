# -*- coding: utf-8 -*-
"""主机 GCC 工具链 —— 用于环境验证 (host build)。

非交叉编译: 用系统 gcc 真实编译各源码模块为 .o, 验证源码树 + 框架 include 连通性
(不依赖 TriCore 工具链)。配合 platform=host 使用; 通常只构建 `compile` 目标。
"""
import os


def configure(env):
    prefix = os.environ.get('GCC_PREFIX', '')
    env['CC']      = prefix + 'gcc'
    env['CXX']     = prefix + 'g++'
    env['AS']      = prefix + 'gcc'
    env['AR']      = prefix + 'ar'
    env['LINK']    = prefix + 'gcc'
    env['RANLIB']  = prefix + 'ranlib'
    env['OBJCOPY'] = prefix + 'objcopy'
    env['OBJDUMP'] = prefix + 'objdump'
    env['SIZE']    = prefix + 'size'

    env.Append(CCFLAGS=[
        '-Wall',
        '-Wno-unused-parameter',
        '-ffunction-sections',
        '-fdata-sections',
        '-std=c99',
    ])
    if env.get('BUILD_TYPE', 'Debug') == 'Release':
        env.Append(CCFLAGS=['-O2'], CPPDEFINES=['NDEBUG'])
    else:
        env.Append(CCFLAGS=['-O0', '-g3'], CPPDEFINES=['DEBUG'])
    env.Append(CCFLAGS=['-MMD'])
    return env
