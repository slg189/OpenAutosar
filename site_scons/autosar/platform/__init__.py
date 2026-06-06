# -*- coding: utf-8 -*-
"""平台（芯片/MCU）工厂。"""
import importlib


def load_platform(env, name: str):
    if not name:
        raise ValueError('未指定 PLATFORM')
    mod_name = name.lower().replace('-', '_').replace('+', 'p')
    try:
        mod = importlib.import_module(f'autosar.platform.{mod_name}')
    except ImportError as e:
        raise ImportError(f'未知平台 "{name}"：{e}')
    return mod.configure(env)
