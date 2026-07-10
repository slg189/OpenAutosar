# -*- coding: utf-8 -*-
"""通用 BSW/MCAL/Component 模块构建器。"""
import os
from SCons.Script import Glob


def build_module(env, module_dir, module_name, mod_cfg=None,
                 extra_src_dirs=None, extra_inc_dirs=None, obj_dir=None):
    """编译一个模块，返回 object 文件列表。

    标准约定：
        {module_dir}/src/*.c           主源码
        {module_dir}/src/*.S           汇编
        {module_dir}/inc/*.h           头文件
        {GEN_DIR}/{module_name}/src/*.c 代码生成器产物
        {GEN_DIR}/{module_name}/inc/*.h 代码生成器头文件

    obj_dir: 若给定, .o 落到该目录 (如 Projects/<P>/Obj/<Layer>/<Mod>), 避免就地污染源码树;
             否则与源码同目录 (回退行为)。
    """
    src_dir     = os.path.join(module_dir, 'src')
    inc_dir     = os.path.join(module_dir, 'inc')
    mod_gen_dir = os.path.join(module_dir, 'gen')          # 模块内的配置/生成码 (如 CDD/<Mod>/gen)
    gen_src_dir = os.path.join(env['GEN_DIR'], module_name, 'src')   # 工程级 Gen/<Mod>/src
    gen_inc_dir = os.path.join(env['GEN_DIR'], module_name, 'inc')

    sources = []
    sources += Glob(os.path.join(src_dir,     '*.c'))
    sources += Glob(os.path.join(src_dir,     '*.S'))
    sources += Glob(os.path.join(mod_gen_dir, '*.c'))
    sources += Glob(os.path.join(mod_gen_dir, '*.S'))
    sources += Glob(os.path.join(gen_src_dir, '*.c'))
    sources += Glob(os.path.join(gen_src_dir, '*.S'))

    for d in (extra_src_dirs or []):
        sources += Glob(os.path.join(d, '*.c'))
        sources += Glob(os.path.join(d, '*.S'))

    extra_objects = []
    if mod_cfg is not None:
        for d in getattr(mod_cfg, 'extra_source_dirs', []) or []:
            sources += Glob(os.path.join(d, '*.c'))
            sources += Glob(os.path.join(d, '*.S'))
        for d in getattr(mod_cfg, 'generated_source_dirs', []) or []:
            sources += Glob(os.path.join(d, '*.c'))
            sources += Glob(os.path.join(d, '*.S'))
        for f in getattr(mod_cfg, 'extra_sources', []) or []:
            sources.append(env.File(f))
        for f in getattr(mod_cfg, 'extra_objects', []) or []:
            extra_objects.append(env.File(f))

    if not sources and not extra_objects:
        print(f'[WARN] {module_name}: 无 .c / .S 源文件，跳过')
        return []

    # 克隆环境以免污染全局
    mod_env = env.Clone()

    # 头文件搜索路径 (含模块内 gen/)
    inc_paths = [inc_dir, mod_gen_dir, gen_inc_dir]
    # BSW 公共头: 本仓为 BSW/inc (兼容旧约定 BSW/_Common/inc)
    for common_inc in (os.path.join(env['BSW_DIR'], 'inc'),
                       os.path.join(env['BSW_DIR'], '_Common', 'inc')):
        if os.path.isdir(common_inc):
            inc_paths.append(common_inc)
    inc_paths += (extra_inc_dirs or [])

    mod_env.Append(CPPPATH=inc_paths)

    # 注入 YAML 中模块级配置
    if mod_cfg is not None:
        if getattr(mod_cfg, 'defines', None):
            mod_env.Append(CPPDEFINES=mod_cfg.defines)
        if getattr(mod_cfg, 'includes', None):
            mod_env.Append(CPPPATH=mod_cfg.includes)
        if getattr(mod_cfg, 'generated_include_dirs', None):
            mod_env.Append(CPPPATH=mod_cfg.generated_include_dirs)
        if getattr(mod_cfg, 'cflags', None):
            mod_env.Append(CCFLAGS=mod_cfg.cflags)

    # 指定 obj_dir 时把 .o 显式落到该目录 (源码为绝对路径, variant_dir 不会自动改写, 故显式指定)
    if obj_dir:
        suffix = mod_env.subst('$OBJSUFFIX') or '.o'
        objs = []
        for s in sources:
            rel = os.path.relpath(str(s), env['ROOT_DIR']).replace('\\', '_').replace('/', '_').replace(':', '')
            base = os.path.splitext(rel)[0]
            objs.append(mod_env.Object(target=os.path.join(obj_dir, base + suffix), source=s))
        return objs + extra_objects
    return mod_env.Object(sources) + extra_objects
