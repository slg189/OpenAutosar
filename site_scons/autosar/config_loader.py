# -*- coding: utf-8 -*-
"""build.yaml 配置加载器。

职责：
    1. 读取 YAML 文件
    2. 递归展开 ${VAR} 占位符
    3. 映射成强类型 dataclass（便于 IDE 提示 + 防拼写错）
"""
import os
import re
from dataclasses import dataclass, field
from typing import List, Dict, Optional

import yaml


class BuildConfigError(ValueError):
    """build.yaml 语义校验失败。"""


# ─────────────────────────────────────────────────────────────
# 数据类
# ─────────────────────────────────────────────────────────────

@dataclass
class ModuleEntry:
    """单个 BSW/MCAL/Component 模块配置。"""
    name:           str
    enabled:        bool          = True
    extra_defines:  List[str]     = field(default_factory=list)
    extra_includes: List[str]     = field(default_factory=list)
    extra_flags:    List[str]     = field(default_factory=list)


@dataclass
class CodegenSpec:
    """单个代码生成器配置。"""
    name:         str
    type:         str            # vector | etas | eb_tresos | turing | custom
    enabled:      bool          = True
    tool_path:    str           = ''
    inputs:       List[str]    = field(default_factory=list)
    outputs_glob: str           = ''
    extra:        Dict          = field(default_factory=dict)


@dataclass
class DocsConfig:
    enabled:            bool   = True
    output_dir:         str    = ''
    doxygen_enabled:    bool   = True
    doxygen_config:     str    = ''
    doxygen_html:       bool   = True
    doxygen_pdf:        bool   = False
    interface_enabled:  bool   = True
    interface_format:   str    = 'markdown'
    dep_graph_enabled:  bool   = True
    dep_graph_format:   str    = 'mermaid'
    module_index:       bool   = True
    index_title:        str    = ''


@dataclass
class RemoteServer:
    name:         str
    host:         str
    port:         int          = 22
    user:         str          = ''
    auth:         str          = 'agent'         # key | password | agent
    key_file:     str          = ''
    remote_dir:   str          = ''
    capabilities: List[str]    = field(default_factory=list)
    jobs:         int          = 8


@dataclass
class RemoteConfig:
    enabled:        bool                 = False
    default_target: str                  = ''
    servers:        List[RemoteServer]   = field(default_factory=list)
    sync_method:    str                  = 'rsync'
    sync_excludes:  List[str]            = field(default_factory=list)
    pull_back:      List[str]            = field(default_factory=list)


@dataclass
class BuildConfig:
    # project
    project_name:     str
    derivative:       str
    app_name_suffix:  str

    # target
    toolchain:        str
    platform:         str
    build_type:       str

    # 全局
    global_defines:   List[str]
    global_includes:  List[str]

    # 模块清单
    bsw_modules:      List[ModuleEntry]
    mcal_modules:     List[ModuleEntry]
    components:       List[ModuleEntry]

    # 链接
    linker_script:    str
    linker_libs:      List[str]
    linker_flags:     List[str]

    # 输出
    output_formats:   List[str]
    size_report:      bool
    compile_commands: bool

    # 代码生成
    codegen_enabled:    bool
    codegen_output_dir: str
    codegen_specs:      List[CodegenSpec]

    # 文档
    docs:             DocsConfig

    # 远程
    remote:           RemoteConfig


# ─────────────────────────────────────────────────────────────
# 变量展开
# ─────────────────────────────────────────────────────────────

_VAR_RE = re.compile(r'\$\{([A-Z_][A-Z0-9_]*)\}')


def _expand(value, variables: Dict[str, str]):
    """递归展开 ${VAR} 占位符。"""
    if isinstance(value, str):
        return _VAR_RE.sub(
            lambda m: variables.get(m.group(1), m.group(0)),
            value,
        )
    if isinstance(value, list):
        return [_expand(v, variables) for v in value]
    if isinstance(value, dict):
        return {k: _expand(v, variables) for k, v in value.items()}
    return value


# ─────────────────────────────────────────────────────────────
# 解析辅助
# ─────────────────────────────────────────────────────────────

def _parse_module(d: dict) -> ModuleEntry:
    if not isinstance(d, dict):
        raise BuildConfigError('module entry must be a mapping')
    if not (d.get('name') or d.get('path')):
        raise BuildConfigError('module entry requires "name" or "path"')
    return ModuleEntry(
        name           = d.get('name') or d.get('path', ''),
        enabled        = bool(d.get('enabled', True)),
        extra_defines  = list(d.get('extra_defines',  []) or []),
        extra_includes = list(d.get('extra_includes', []) or []),
        extra_flags    = list(d.get('extra_flags',    []) or []),
    )


def _parse_codegen_spec(d: dict) -> CodegenSpec:
    if not isinstance(d, dict):
        raise BuildConfigError('codegen generator entry must be a mapping')
    for key in ('name', 'type'):
        if not d.get(key):
            raise BuildConfigError(f'codegen generator requires "{key}"')
    known = {'name', 'type', 'enabled', 'tool_path', 'inputs', 'outputs_glob'}
    extra = {k: v for k, v in d.items() if k not in known}
    return CodegenSpec(
        name         = d['name'],
        type         = d['type'],
        enabled      = bool(d.get('enabled', True)),
        tool_path    = d.get('tool_path', ''),
        inputs       = list(d.get('inputs', []) or []),
        outputs_glob = d.get('outputs_glob', ''),
        extra        = extra,
    )


def _parse_docs(d: dict) -> DocsConfig:
    if not d:
        return DocsConfig(enabled=False)
    dox  = d.get('doxygen',          {}) or {}
    iface= d.get('interface_extract',{}) or {}
    dep  = d.get('dependency_graph', {}) or {}
    idx  = d.get('module_index',     {}) or {}
    return DocsConfig(
        enabled           = bool(d.get('enabled', True)),
        output_dir        = d.get('output_dir', ''),
        doxygen_enabled   = bool(dox.get('enabled', True)),
        doxygen_config    = dox.get('config_file', ''),
        doxygen_html      = bool(dox.get('html', True)),
        doxygen_pdf       = bool(dox.get('pdf',  False)),
        interface_enabled = bool(iface.get('enabled', True)),
        interface_format  = iface.get('output_format', 'markdown'),
        dep_graph_enabled = bool(dep.get('enabled', True)),
        dep_graph_format  = dep.get('format', 'mermaid'),
        module_index      = bool(idx.get('enabled', True)),
        index_title       = idx.get('title', ''),
    )


def _parse_remote(d: dict) -> RemoteConfig:
    if not d:
        return RemoteConfig()
    sync = d.get('sync', {}) or {}
    servers = [
        RemoteServer(
            name         = s['name'],
            host         = s['host'],
            port         = int(s.get('port', 22)),
            user         = s.get('user', ''),
            auth         = s.get('auth', 'agent'),
            key_file     = s.get('key_file', ''),
            remote_dir   = s.get('remote_dir', ''),
            capabilities = list(s.get('capabilities', []) or []),
            jobs         = int(s.get('jobs', 8)),
        )
        for s in d.get('servers', []) or []
    ]
    return RemoteConfig(
        enabled        = bool(d.get('enabled', False)),
        default_target = d.get('default_target', ''),
        servers        = servers,
        sync_method    = sync.get('method', 'rsync'),
        sync_excludes  = list(sync.get('excludes', []) or []),
        pull_back      = list(sync.get('pull_back', []) or []),
    )


def _require_mapping(raw, key):
    value = raw.get(key)
    if not isinstance(value, dict):
        raise BuildConfigError(f'build.yaml requires mapping node "{key}"')
    return value


def _require_string(node, key, section):
    value = node.get(key)
    if not isinstance(value, str) or not value:
        raise BuildConfigError(f'{section}.{key} is required and must be a non-empty string')
    return value


def _validate_known_top_level(raw):
    known = {
        'schema_version', 'project', 'target', 'global_defines', 'global_includes',
        'bsw_modules', 'mcal_modules', 'components', 'linker', 'output',
        'codegen', 'docs', 'remote',
    }
    unknown = sorted(set(raw.keys()) - known)
    if unknown:
        raise BuildConfigError(f'unknown top-level keys: {", ".join(unknown)}')


def _validate_list(raw, key):
    value = raw.get(key, [])
    if value is None:
        return []
    if not isinstance(value, list):
        raise BuildConfigError(f'{key} must be a list')
    return value


# ─────────────────────────────────────────────────────────────
# 主入口
# ─────────────────────────────────────────────────────────────

def load(yaml_path: str, variables: Optional[Dict[str, str]] = None) -> BuildConfig:
    """读取 build.yaml → 展开变量 → 返回 BuildConfig。"""
    variables = variables or {}

    with open(yaml_path, encoding='utf-8') as f:
        raw = yaml.safe_load(f)
    if not isinstance(raw, dict):
        raise BuildConfigError(f'{yaml_path} top-level node must be a YAML mapping')
    _validate_known_top_level(raw)

    schema_version = str(raw.get('schema_version', '2'))
    if schema_version.split('.')[0] != '2':
        raise BuildConfigError(f'unsupported schema_version "{schema_version}", expected v2.x')

    # 把 project 字段也塞进 variables 以便后续 ${PROJECT_NAME} 可用
    proj = _require_mapping(raw, 'project')
    _require_string(proj, 'name', 'project')
    variables = dict(variables)
    variables.setdefault('PROJECT_NAME', proj.get('name', ''))
    variables.setdefault('DERIVATIVE',   proj.get('derivative', ''))

    raw = _expand(raw, variables)

    target = _require_mapping(raw, 'target')
    _require_string(target, 'toolchain', 'target')
    _require_string(target, 'platform', 'target')
    linker = raw.get('linker', {}) or {}
    output = raw.get('output', {}) or {}
    codegen= raw.get('codegen', {}) or {}

    return BuildConfig(
        project_name     = proj['name'],
        derivative       = proj.get('derivative', ''),
        app_name_suffix  = proj.get('app_name_suffix', ''),

        toolchain        = target['toolchain'],
        platform         = target['platform'],
        build_type       = target.get('build_type', 'Debug'),

        global_defines   = _validate_list(raw, 'global_defines'),
        global_includes  = _validate_list(raw, 'global_includes'),

        bsw_modules      = [_parse_module(m) for m in _validate_list(raw, 'bsw_modules')],
        mcal_modules     = [_parse_module(m) for m in _validate_list(raw, 'mcal_modules')],
        components       = [_parse_module(m) for m in _validate_list(raw, 'components')],

        linker_script    = linker.get('script', ''),
        linker_libs      = list(linker.get('libraries',   []) or []),
        linker_flags     = list(linker.get('extra_flags', []) or []),

        output_formats   = list(output.get('formats', ['elf']) or ['elf']),
        size_report      = bool(output.get('size_report', True)),
        compile_commands = bool(output.get('compile_commands_json', True)),

        codegen_enabled    = bool(codegen.get('enabled', False)),
        codegen_output_dir = codegen.get('output_dir', ''),
        codegen_specs      = [_parse_codegen_spec(s)
                              for s in codegen.get('generators', []) or []],

        docs              = _parse_docs(raw.get('docs')),
        remote            = _parse_remote(raw.get('remote')),
    )
