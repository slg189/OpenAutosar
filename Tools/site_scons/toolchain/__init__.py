# -*- coding: utf-8 -*-
"""工具链工厂。"""
import importlib


def load_toolchain(env, name: str):
    """根据名称加载对应工具链并返回配置好的 env。"""
    if not name:
        raise ValueError('未指定 TOOLCHAIN')
    try:
        mod = importlib.import_module(f'site_scons.toolchain.{name.lower()}')
    except ImportError as e:
        raise ImportError(f'未知工具链 "{name}"：{e}')
    return mod.configure(env)
