# -*- coding: utf-8 -*-
"""build.yaml 配置加载器 (schema 3.0)。

v3 模型: 统一的 layers/mode —— 每层每模块用同一个 mode(source|lib) 表达,
配合 defaults.visibility=auto 让框架按"工作区是否可见源码"自动决定编译还是链接。

职责:
    1. 读取 YAML + 递归展开 ${VAR}
    2. schema 3.0 基础校验 (未知顶层键 / 必填字段)
    3. 映射为强类型 dataclass
"""
import re
from dataclasses import dataclass, field
from typing import List, Dict, Optional

import yaml


class BuildConfigError(ValueError):
    """build.yaml 语义校验失败。"""


# ───────────────────────── 数据类 ─────────────────────────

@dataclass
class ModuleOverride:
    """build.yaml 中对某模块的"例外"声明 (其余靠自动发现 + defaults)。"""
    name:     str
    enabled:  bool      = True
    mode:     str       = ''            # '' = 跟随 defaults.visibility; 'source' | 'lib'
    lib_name: str       = ''            # 覆盖 .a 基名 (默认 lib<leaf>, 如 Os->libRtaOs)
    cflags:   List[str] = field(default_factory=list)
    defines:  List[str] = field(default_factory=list)
    includes: List[str] = field(default_factory=list)
    release_includes: List[str] = field(default_factory=list)  # release 时额外发布到 *_Libs 的头文件路径 (支持 ${VAR})


@dataclass
class CodegenSpec:
    name: str
    type: str
    enabled: bool = True
    tool_path: str = ''
    inputs: List[str] = field(default_factory=list)
    outputs_glob: str = ''
    extra: Dict = field(default_factory=dict)


@dataclass
class DocsConfig:
    enabled: bool = True
    output_dir: str = ''
    doxygen_enabled: bool = False
    doxygen_config: str = ''
    doxygen_html: bool = True
    doxygen_pdf: bool = False
    interface_enabled: bool = True
    interface_format: str = 'markdown'
    dep_graph_enabled: bool = True
    dep_graph_format: str = 'mermaid'
    module_index: bool = True
    index_title: str = ''


@dataclass
class RemoteServer:
    name: str
    host: str
    port: int = 22
    user: str = ''
    auth: str = 'agent'
    key_file: str = ''
    remote_dir: str = ''
    capabilities: List[str] = field(default_factory=list)
    jobs: int = 8


@dataclass
class RemoteConfig:
    enabled: bool = False
    default_target: str = ''
    servers: List[RemoteServer] = field(default_factory=list)
    sync_method: str = 'rsync'
    sync_excludes: List[str] = field(default_factory=list)
    pull_back: List[str] = field(default_factory=list)


@dataclass
class BuildConfig:
    # project
    project_name: str
    derivative: str
    app_name_suffix: str
    # target
    toolchain: str
    platform: str
    build_type: str
    # defaults
    visibility: str          # auto | source | lib
    chip: str
    # 全局
    global_defines: List[str]
    global_includes: List[str]
    # ★ 分层模块 (显式清单, 自动发现补全): layer -> {modname: ModuleOverride}
    layers: Dict[str, Dict[str, ModuleOverride]]
    # 集成层
    integration_enabled: bool
    integration_dir: str
    # 链接
    linker_script: str
    linker_libs: List[str]
    linker_flags: List[str]
    # 输出
    output_formats: List[str]
    size_report: bool
    compile_commands: bool
    # 能力插件
    codegen_enabled: bool
    codegen_output_dir: str
    codegen_specs: List[CodegenSpec]
    docs: DocsConfig
    remote: RemoteConfig


# ───────────────────────── 变量展开 ─────────────────────────

_VAR_RE = re.compile(r'\$\{([A-Z_][A-Z0-9_]*)\}')


def _expand(value, variables):
    if isinstance(value, str):
        return _VAR_RE.sub(lambda m: variables.get(m.group(1), m.group(0)), value)
    if isinstance(value, list):
        return [_expand(v, variables) for v in value]
    if isinstance(value, dict):
        return {k: _expand(v, variables) for k, v in value.items()}
    return value


# ───────────────────────── 解析辅助 ─────────────────────────

def _require_mapping(raw, key):
    v = raw.get(key)
    if not isinstance(v, dict):
        raise BuildConfigError(f'build.yaml 缺少 mapping 节点 "{key}"')
    return v


def _require_string(node, key, section):
    v = node.get(key)
    if not isinstance(v, str) or not v:
        raise BuildConfigError(f'{section}.{key} 必填且为非空字符串')
    return v


def _validate_known_top_level(raw):
    known = {
        'schema_version', 'project', 'target', 'defaults',
        'global_defines', 'global_includes', 'layers', 'integration',
        'linker', 'output', 'codegen', 'docs', 'remote',
    }
    unknown = sorted(set(raw.keys()) - known)
    if unknown:
        raise BuildConfigError(f'未知顶层键: {", ".join(unknown)}')


def _as_list(raw, key):
    v = raw.get(key, [])
    if v is None:
        return []
    if not isinstance(v, list):
        raise BuildConfigError(f'{key} 必须是 list')
    return v


def _parse_module(name, d) -> ModuleOverride:
    d = d or {}
    if not isinstance(d, dict):
        raise BuildConfigError(f'模块 "{name}" 配置必须是 mapping 或空')
    mode = d.get('mode', '')
    if mode not in ('', 'source', 'lib'):
        raise BuildConfigError(f'模块 "{name}" mode 只能为 source|lib (得到 {mode!r})')
    return ModuleOverride(
        name=name,
        enabled=bool(d.get('enabled', True)),
        mode=mode,
        lib_name=d.get('lib_name', ''),
        cflags=list(d.get('cflags', []) or []),
        defines=list(d.get('defines', []) or []),
        includes=list(d.get('includes', []) or []),
        release_includes=list(d.get('release_includes', []) or []),
    )


def _parse_layers(raw) -> Dict[str, Dict[str, ModuleOverride]]:
    layers_raw = raw.get('layers', {}) or {}
    if not isinstance(layers_raw, dict):
        raise BuildConfigError('layers 必须是 mapping')
    known_layers = {'ASW', 'CDD', 'MCAL', 'BSW', 'Components'}
    out = {}
    for layer, mods in layers_raw.items():
        if layer not in known_layers:
            raise BuildConfigError(f'未知层 "{layer}" (应为 {sorted(known_layers)})')
        mods = mods or {}
        if not isinstance(mods, dict):
            raise BuildConfigError(f'层 "{layer}" 下应为 mapping(模块名->配置)')
        out[layer] = {name: _parse_module(name, cfg) for name, cfg in mods.items()}
    return out


def _parse_codegen_spec(d) -> CodegenSpec:
    if not isinstance(d, dict):
        raise BuildConfigError('codegen generator 必须是 mapping')
    for k in ('name', 'type'):
        if not d.get(k):
            raise BuildConfigError(f'codegen generator 缺少 "{k}"')
    known = {'name', 'type', 'enabled', 'tool_path', 'inputs', 'outputs_glob'}
    return CodegenSpec(
        name=d['name'], type=d['type'], enabled=bool(d.get('enabled', True)),
        tool_path=d.get('tool_path', ''), inputs=list(d.get('inputs', []) or []),
        outputs_glob=d.get('outputs_glob', ''),
        extra={k: v for k, v in d.items() if k not in known},
    )


def _parse_docs(d) -> DocsConfig:
    if not d:
        return DocsConfig(enabled=False)
    dox = d.get('doxygen', {}) or {}
    iface = d.get('interface_extract', {}) or {}
    dep = d.get('dependency_graph', {}) or {}
    idx = d.get('module_index', {}) or {}
    return DocsConfig(
        enabled=bool(d.get('enabled', True)), output_dir=d.get('output_dir', ''),
        doxygen_enabled=bool(dox.get('enabled', False)), doxygen_config=dox.get('config_file', ''),
        doxygen_html=bool(dox.get('html', True)), doxygen_pdf=bool(dox.get('pdf', False)),
        interface_enabled=bool(iface.get('enabled', True)), interface_format=iface.get('output_format', 'markdown'),
        dep_graph_enabled=bool(dep.get('enabled', True)), dep_graph_format=dep.get('format', 'mermaid'),
        module_index=bool(idx.get('enabled', True)), index_title=idx.get('title', ''),
    )


def _parse_remote(d) -> RemoteConfig:
    if not d:
        return RemoteConfig()
    sync = d.get('sync', {}) or {}
    servers = [
        RemoteServer(
            name=s['name'], host=s['host'], port=int(s.get('port', 22)),
            user=s.get('user', ''), auth=s.get('auth', 'agent'), key_file=s.get('key_file', ''),
            remote_dir=s.get('remote_dir', ''), capabilities=list(s.get('capabilities', []) or []),
            jobs=int(s.get('jobs', 8)),
        ) for s in d.get('servers', []) or []
    ]
    return RemoteConfig(
        enabled=bool(d.get('enabled', False)), default_target=d.get('default_target', ''),
        servers=servers, sync_method=sync.get('method', 'rsync'),
        sync_excludes=list(sync.get('excludes', []) or []), pull_back=list(sync.get('pull_back', []) or []),
    )


# ───────────────────────── 主入口 ─────────────────────────

def app_name(cfg: 'BuildConfig') -> str:
    """工程产物基名: <project.name>_<derivative>[-<app_name_suffix>] (与 SConscript 一致)。

    供独立脚本 (集成测试等) 从 build.yaml 推导 ELF 名, 不再假设 = 工程目录名。
    """
    name = f'{cfg.project_name}_{cfg.derivative}' if cfg.derivative else cfg.project_name
    if cfg.app_name_suffix:
        name += f'-{cfg.app_name_suffix}'
    return name


def load(yaml_path: str, variables: Optional[Dict[str, str]] = None) -> BuildConfig:
    variables = dict(variables or {})
    with open(yaml_path, encoding='utf-8') as f:
        raw = yaml.safe_load(f)
    if not isinstance(raw, dict):
        raise BuildConfigError(f'{yaml_path} 顶层必须是 YAML mapping')
    _validate_known_top_level(raw)

    sv = str(raw.get('schema_version', '3'))
    if sv.split('.')[0] != '3':
        raise BuildConfigError(f'不支持的 schema_version "{sv}", 期望 v3.x (v3 不兼容 v2)')

    proj = _require_mapping(raw, 'project')
    _require_string(proj, 'name', 'project')
    variables.setdefault('PROJECT_NAME', proj.get('name', ''))
    variables.setdefault('DERIVATIVE', proj.get('derivative', ''))
    raw = _expand(raw, variables)

    target = _require_mapping(raw, 'target')
    _require_string(target, 'toolchain', 'target')
    _require_string(target, 'platform', 'target')
    defaults = raw.get('defaults', {}) or {}
    vis = defaults.get('visibility', 'auto')
    if vis not in ('auto', 'source', 'lib'):
        raise BuildConfigError(f'defaults.visibility 只能为 auto|source|lib (得到 {vis!r})')
    integ = raw.get('integration', {}) or {}
    linker = raw.get('linker', {}) or {}
    output = raw.get('output', {}) or {}
    codegen = raw.get('codegen', {}) or {}

    return BuildConfig(
        project_name=proj['name'], derivative=proj.get('derivative', ''),
        app_name_suffix=proj.get('app_name_suffix', ''),
        toolchain=target['toolchain'], platform=target['platform'],
        build_type=target.get('build_type', 'Debug'),
        visibility=vis, chip=defaults.get('chip', proj.get('derivative', '')),
        global_defines=_as_list(raw, 'global_defines'),
        global_includes=_as_list(raw, 'global_includes'),
        layers=_parse_layers(raw),
        integration_enabled=bool(integ.get('enabled', True)),
        integration_dir=integ.get('dir', 'Integration'),
        linker_script=linker.get('script', ''),
        linker_libs=list(linker.get('libraries', []) or []),
        linker_flags=list(linker.get('extra_flags', []) or []),
        output_formats=list(output.get('formats', ['elf']) or ['elf']),
        size_report=bool(output.get('size_report', True)),
        compile_commands=bool(output.get('compile_commands_json', True)),
        codegen_enabled=bool(codegen.get('enabled', False)),
        codegen_output_dir=codegen.get('output_dir', ''),
        codegen_specs=[_parse_codegen_spec(s) for s in codegen.get('generators', []) or []],
        docs=_parse_docs(raw.get('docs')),
        remote=_parse_remote(raw.get('remote')),
    )
