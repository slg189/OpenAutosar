# -*- coding: utf-8 -*-
"""source / lib 决策器 (含信息安全 *_Libs 集成)。

对每个模块决定: 从源码编译, 还是链接预编译 .a。
判定输入:
    1. build.yaml 的显式 mode (source|lib)
    2. defaults.visibility (auto|source|lib)
    3. 工作区里源码是否可见 (manifest 决定) —— auto 时据此自动选择
lib 来源按顺序找:
    a. <Layer>_Libs/<mod>/lib<name>.a   (信息安全释放区, 首选)
    b. <Layer>/<mod>/lib<name>.a        (模块目录内的预编译交付)
"""
import os
from dataclasses import dataclass, field
from typing import List

LAYER_LIB_DIR = {
    'ASW': 'ASW_Libs', 'CDD': 'CDD_Libs', 'BSW': 'BSW_Libs', 'MCAL': 'MCAL_Libs',
}


@dataclass
class Decision:
    kind: str                 # 'source' | 'lib' | 'error'
    layer: str = ''
    modname: str = ''
    leaf: str = ''
    src_dir: str = ''         # kind=source
    a_file: str = ''          # kind=lib
    inc_dirs: List[str] = field(default_factory=list)
    reason: str = ''          # 决策原因 (供 --explain)
    tried: List[str] = field(default_factory=list)


def _existing(dirs):
    return [d for d in dirs if os.path.isdir(d)]


def resolve(ws, layer, ov, defaults_visibility) -> Decision:
    """ws: Workspace; ov: ModuleOverride; 返回 Decision。"""
    modname = ov.name
    leaf = modname.split('/')[-1]
    mod_dir = ws.module_dir(layer, modname)
    visible = ws.source_visible(layer, modname)
    mode = ov.mode or defaults_visibility

    # ---- 源码编译 ----
    if mode == 'source' or (mode == 'auto' and visible):
        if not visible:
            return Decision('error', layer, modname, leaf,
                            reason=f'声明 source 但工作区无源码: {mod_dir}/src/*.c')
        return Decision('source', layer, modname, leaf, src_dir=mod_dir,
                        reason=('显式 source' if ov.mode else 'auto: 源码可见'))

    # ---- 链接预编译库 ----
    libbase = ov.lib_name or ('lib' + leaf)
    tried = []
    candidates = []
    if layer in LAYER_LIB_DIR:
        lib_root = os.path.join(ws.root, LAYER_LIB_DIR[layer], modname)
        candidates.append((os.path.join(lib_root, libbase + '.a'),
                           [os.path.join(ws.root, LAYER_LIB_DIR[layer], 'inc'),
                            os.path.join(lib_root, 'inc')]))
    candidates.append((os.path.join(mod_dir, libbase + '.a'),
                       [os.path.join(mod_dir, 'inc')]))
    for a_file, inc in candidates:
        tried.append(a_file)
        if os.path.exists(a_file):
            reason = ('显式 lib' if ov.mode == 'lib'
                      else ('auto: 源码不可见, 走库' if mode == 'auto' else 'defaults: lib'))
            return Decision('lib', layer, modname, leaf, a_file=a_file,
                            inc_dirs=_existing(inc), reason=reason, tried=tried)

    return Decision('error', layer, modname, leaf, tried=tried,
                    reason='源码不可见且未找到释放库 (*_Libs 或模块内 .a)')
