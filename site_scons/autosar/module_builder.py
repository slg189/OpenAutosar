# -*- coding: utf-8 -*-
"""通用 BSW/MCAL/Component 模块构建器。"""
import os
from SCons.Script import Glob


def build_module(env, module_dir, module_name, mod_cfg=None,
                 extra_src_dirs=None, extra_inc_dirs=None):
    """编译一个模块，返回 object 文件列表。

    标准约定：
        {module_dir}/src/*.c           主源码
        {module_dir}/src/*.S           汇编
        {module_dir}/inc/*.h           头文件
        {GEN_DIR}/{module_name}/src/*.c 代码生成器产物
        {GEN_DIR}/{module_name}/inc/*.h 代码生成器头文件
    """
    src_dir     = os.path.join(module_dir, 'src')
    inc_dir     = os.path.join(module_dir, 'inc')
    gen_src_dir = os.path.join(env['GEN_DIR'], module_name, 'src')
    gen_inc_dir = os.path.join(env['GEN_DIR'], module_name, 'inc')

    sources = []
    sources += Glob(os.path.join(src_dir,     '*.c'))
    sources += Glob(os.path.join(src_dir,     '*.S'))
    sources += Glob(os.path.join(gen_src_dir, '*.c'))

    for d in (extra_src_dirs or []):
        sources += Glob(os.path.join(d, '*.c'))
        sources += Glob(os.path.join(d, '*.S'))

    if not sources:
        print(f'[WARN] {module_name}: 无 .c / .S 源文件，跳过')
        return []

    # 克隆环境以免污染全局
    mod_env = env.Clone()

    # 头文件搜索路径
    inc_paths = [inc_dir, gen_inc_dir]
    # BSW 公共头: 本仓为 BSW/inc (兼容旧约定 BSW/_Common/inc)
    for common_inc in (os.path.join(env['BSW_DIR'], 'inc'),
                       os.path.join(env['BSW_DIR'], '_Common', 'inc')):
        if os.path.isdir(common_inc):
            inc_paths.append(common_inc)
    inc_paths += (extra_inc_dirs or [])

    mod_env.Append(CPPPATH=inc_paths)

    # 注入 YAML 中模块级配置
    if mod_cfg is not None:
        if mod_cfg.extra_defines:
            mod_env.Append(CPPDEFINES=mod_cfg.extra_defines)
        if mod_cfg.extra_includes:
            mod_env.Append(CPPPATH=mod_cfg.extra_includes)
        if mod_cfg.extra_flags:
            mod_env.Append(CCFLAGS=mod_cfg.extra_flags)

    return mod_env.Object(sources)
