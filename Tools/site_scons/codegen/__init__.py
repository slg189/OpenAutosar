# -*- coding: utf-8 -*-
"""代码生成器工厂。"""
import importlib

_REGISTRY = {
    'vector':    'site_scons.codegen.vector:VectorAdapter',
    'etas':      'site_scons.codegen.etas:EtasAdapter',
    'eb_tresos': 'site_scons.codegen.eb_tresos:EbTresosAdapter',
    'turing':    'site_scons.codegen.turing:TuringAdapter',
    'custom':    'site_scons.codegen.custom:CustomAdapter',
}


def create_adapter(type_name: str):
    if type_name not in _REGISTRY:
        raise ValueError(f'未知 codegen type："{type_name}"，'
                         f'已注册：{sorted(_REGISTRY.keys())}')
    mod_path, cls_name = _REGISTRY[type_name].split(':')
    mod = importlib.import_module(mod_path)
    return getattr(mod, cls_name)()
