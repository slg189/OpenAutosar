# -*- coding: utf-8 -*-
"""source / lib 决策器 (含信息安全 *_Libs 集成)。

对每个模块决定: 从源码编译, 链接预编译 .a, 还是仅提供头文件。
判定输入:
    1. build.yaml 的显式 mode (source|lib)
    2. defaults.visibility (auto|source|lib)
    3. 工作区里源码是否可见 (manifest 决定) —— auto 时据此自动选择
lib 来源按顺序找:
    a. <Layer>_Libs/<mod>/lib<name>.a   (信息安全释放区, 首选)
    b. <Layer>/<mod>/lib<name>.a        (模块目录内的预编译交付)
                                        —— 此情形 release 时需把 .a + inc 更新到 <Layer>_Libs
三种结果:
    source —— 有 .c/.S, 从源码编译 (release 归档 .a + inc 到 <Layer>_Libs)
    lib    —— 有预编译 .a, 直接链接 (lib_from_module=True 时 release 把 .a+inc 拷到 <Layer>_Libs)
    header —— 只有 .h、无 .c 也无 .a (仅提供 include 路径, 不编译不链接)
"""
import os
from dataclasses import dataclass, field
from typing import List

LAYER_LIB_DIR = {
    'ASW': 'ASW_Libs', 'CDD': 'CDD_Libs', 'BSW': 'BSW_Libs', 'MCAL': 'MCAL_Libs',
}


@dataclass
class Decision:
    kind: str                 # 'source' | 'lib' | 'header' | 'error'
    layer: str = ''
    modname: str = ''
    leaf: str = ''
    src_dir: str = ''         # kind=source/header (模块目录)
    a_file: str = ''          # kind=lib
    inc_dirs: List[str] = field(default_factory=list)  # kind=lib/header 对外头文件目录
    lib_from_module: bool = False  # kind=lib: .a 取自 <Layer>/<mod>/ (非 *_Libs), release 需发布
    reason: str = ''          # 决策原因 (供 --explain)
    tried: List[str] = field(default_factory=list)


def _existing(dirs):
    return [d for d in dirs if os.path.isdir(d)]


def _has_headers(d):
    inc = os.path.join(d, 'inc')
    if os.path.isdir(inc):
        for f in os.listdir(inc):
            if f.endswith('.h'):
                return True
    return False


def _has_configured_build_inputs(mod_dir, ov):
    if getattr(ov, 'extra_sources', None) or getattr(ov, 'extra_source_dirs', None):
        return True
    if getattr(ov, 'extra_objects', None) or getattr(ov, 'generated_source_dirs', None):
        return True
    gen_dir = os.path.join(mod_dir, 'gen')
    if os.path.isdir(gen_dir):
        for f in os.listdir(gen_dir):
            if f.endswith(('.c', '.S', '.o')):
                return True
    return False


def resolve(ws, layer, ov, defaults_visibility) -> Decision:
    """ws: Workspace; ov: ModuleOverride; 返回 Decision。"""
    modname = ov.name
    leaf = modname.split('/')[-1]
    mod_dir = ws.module_dir(layer, modname)
    visible = ws.source_visible(layer, modname) or _has_configured_build_inputs(mod_dir, ov)
    mode = ov.mode or defaults_visibility

    # ---- 源码编译 ----
    if mode == 'source' or (mode == 'auto' and visible):
        if visible:
            return Decision('source', layer, modname, leaf, src_dir=mod_dir,
                            reason=('显式 source' if ov.mode else 'auto: 源码可见'))
        # 声明/auto 走 source, 但无 .c/.S: 若只有头文件 -> header-only (不报错, 只给 include)
        if _has_headers(mod_dir):
            return Decision('header', layer, modname, leaf, src_dir=mod_dir,
                            inc_dirs=_existing([os.path.join(mod_dir, 'inc')]),
                            reason='仅头文件 (无 .c/.S), 作为 include-only')
        return Decision('error', layer, modname, leaf,
                        reason=f'声明 source 但工作区无源码: {mod_dir}/src/*.c')

    # ---- 链接预编译库 ----
    libbase = ov.lib_name or ('lib' + leaf)
    tried = []
    candidates = []  # (a_file, inc_dirs, from_module)
    if layer in LAYER_LIB_DIR:
        lib_root = os.path.join(ws.root, LAYER_LIB_DIR[layer], modname)
        candidates.append((os.path.join(lib_root, libbase + '.a'),
                           [os.path.join(ws.root, LAYER_LIB_DIR[layer], 'inc'),
                            os.path.join(lib_root, 'inc')], False))
    candidates.append((os.path.join(mod_dir, libbase + '.a'),
                       [os.path.join(mod_dir, 'inc')], True))
    for a_file, inc, from_module in candidates:
        tried.append(a_file)
        if os.path.exists(a_file):
            reason = ('显式 lib' if ov.mode == 'lib'
                      else ('auto: 源码不可见, 走库' if mode == 'auto' else 'defaults: lib'))
            if from_module:
                reason += ' (取自模块目录, release 将发布到 *_Libs)'
            return Decision('lib', layer, modname, leaf, a_file=a_file, src_dir=mod_dir,
                            inc_dirs=_existing(inc), lib_from_module=from_module,
                            reason=reason, tried=tried)

    # ---- 无 .a 兜底: 只有头文件 -> header-only ----
    if _has_headers(mod_dir):
        return Decision('header', layer, modname, leaf, src_dir=mod_dir,
                        inc_dirs=_existing([os.path.join(mod_dir, 'inc')]),
                        reason='未找到 .a, 但有头文件, 作为 include-only')

    return Decision('error', layer, modname, leaf, tried=tried,
                    reason='源码不可见且未找到释放库 (*_Libs 或模块内 .a)')
