# -*- coding: utf-8 -*-
"""工作区与模块自动发现 (约定优于配置)。

约定: 一个"模块" = 某层根下含 inc/ 或 src/ 的目录。
    - MCAL 按芯片再分一级:  MCAL/<chip>/<Mod>
    - 其余层:               <Layer>/<Mod>   (Components 允许嵌套, 如 Mcu/Sbm)

可见性 (信息安全): 模块源码"可见" = 工作区里真有 <dir>/src/*.c
    (由 google-repo manifest 决定签出哪些层的源码仓)。
"""
import glob
import os

LAYER_DIR = {
    'ASW': 'ASW', 'CDD': 'CDD', 'MCAL': 'MCAL', 'BSW': 'BSW', 'Components': 'Components',
}


def _has_c(d):
    return bool(glob.glob(os.path.join(d, 'src', '*.c')) or glob.glob(os.path.join(d, 'src', '*.S')))


def _is_module_dir(d):
    return os.path.isdir(os.path.join(d, 'inc')) or os.path.isdir(os.path.join(d, 'src'))


class Workspace:
    """封装工作区根 + 当前芯片, 提供模块发现与路径/可见性解析。"""

    def __init__(self, root, chip=''):
        self.root = root
        self.chip = chip

    # ---- 路径 ----
    def layer_root(self, layer):
        sub = LAYER_DIR[layer]
        if layer == 'MCAL' and self.chip:
            return os.path.join(self.root, sub, self.chip)
        return os.path.join(self.root, sub)

    def module_dir(self, layer, modname):
        return os.path.join(self.layer_root(layer), modname)

    # ---- 发现 ----
    def discover(self):
        """返回 {layer: {modname: abs_dir}}。modname 可含子路径 (如 Mcu/Sbm)。"""
        found = {}
        for layer in LAYER_DIR:
            root = self.layer_root(layer)
            mods = {}
            if os.path.isdir(root):
                self._scan(root, root, mods)
            found[layer] = mods
        return found

    def _scan(self, base, cur, mods, depth=0):
        for name in sorted(os.listdir(cur)):
            p = os.path.join(cur, name)
            if not os.path.isdir(p) or name in ('inc', 'src', 'gen'):
                continue
            if _is_module_dir(p):
                rel = os.path.relpath(p, base).replace('\\', '/')
                mods[rel] = p
            elif depth < 2:
                self._scan(base, p, mods, depth + 1)   # 容器目录 (如 Components/Mcu) 继续下钻

    # ---- 可见性 ----
    def source_visible(self, layer, modname):
        return _has_c(self.module_dir(layer, modname))

    def undeclared(self, declared_layers):
        """工作区里存在、但 build.yaml 未显式声明的模块 (供 scons list 提示)。"""
        out = {}
        disc = self.discover()
        for layer, mods in disc.items():
            decl = set((declared_layers.get(layer) or {}).keys())
            extra = sorted(set(mods.keys()) - decl)
            if extra:
                out[layer] = extra
        return out
