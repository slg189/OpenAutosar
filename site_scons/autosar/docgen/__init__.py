# -*- coding: utf-8 -*-
"""文档生成入口 (层无关: 接口抽取 / 依赖图 / 索引 / 可选 Doxygen)。"""
import glob
import os
import re

from .interface_extractor import extract_module_interfaces, render_markdown
from .dependency_grapher import render_mermaid
from .doxygen_runner import run_doxygen
from .index_builder import build_index_html

_INC_RE = re.compile(r'#\s*include\s+"([^"]+\.h)"')


def _infer_deps(module_dirs):
    """按 #include "X.h" 推断模块间依赖 (X.h 属于哪个模块)。"""
    header_owner = {}
    for name, d in module_dirs.items():
        for h in glob.glob(os.path.join(d, 'inc', '*.h')):
            header_owner[os.path.basename(h)] = name
    deps = {}
    for name, d in module_dirs.items():
        used = set()
        for c in glob.glob(os.path.join(d, 'src', '*.c')):
            try:
                txt = open(c, encoding='utf-8', errors='ignore').read()
            except OSError:
                continue
            for inc in _INC_RE.findall(txt):
                owner = header_owner.get(os.path.basename(inc))
                if owner and owner != name:
                    used.add(owner)
        deps[name] = sorted(used)
    return deps


def build_docs(env, cfg, modules=None, module_dirs=None,
               target_name='doc', include_site=True):
    """生成文档。modules: 含 .name/.enabled 的对象列表; module_dirs: {name: 绝对目录}。"""
    modules = modules or []
    module_dirs = module_dirs or {}
    out_dir = cfg.docs.output_dir or os.path.join(env['ROOT_DIR'], 'docs', 'api')
    os.makedirs(os.path.join(out_dir, 'modules'), exist_ok=True)
    os.makedirs(os.path.join(out_dir, 'graphs'), exist_ok=True)

    def _generate_docs(target, source, env_):
        # 1) 接口抽取 (每模块一份 markdown)
        if cfg.docs.interface_enabled:
            for name, mdir in module_dirs.items():
                if not os.path.isdir(mdir):
                    continue
                ifaces = extract_module_interfaces(mdir)
                md = render_markdown(name, ifaces)
                mod_out = os.path.join(out_dir, 'modules', name.replace('/', '_'))
                os.makedirs(mod_out, exist_ok=True)
                with open(os.path.join(mod_out, 'interfaces.md'), 'w', encoding='utf-8') as f:
                    f.write(md)
        # 2) 依赖图
        if include_site and cfg.docs.dep_graph_enabled:
            deps = _infer_deps(module_dirs)
            mermaid = render_mermaid(
                [{'name': m.name, 'enabled': getattr(m, 'enabled', True)} for m in modules], deps)
            with open(os.path.join(out_dir, 'graphs', 'dependency.mmd'), 'w', encoding='utf-8') as f:
                f.write(mermaid)
        # 3) Doxygen (可选, 默认关)
        if cfg.docs.doxygen_enabled and cfg.docs.doxygen_config:
            run_doxygen(config_template=cfg.docs.doxygen_config, modules=modules,
                        bsw_dir=env['BSW_DIR'], output_dir=out_dir)
        # 4) 索引页
        if include_site and cfg.docs.module_index:
            build_index_html(output_dir=out_dir,
                             title=cfg.docs.index_title or f'{cfg.project_name} 模块清单',
                             project=cfg.project_name, modules=modules, all_bsw=modules)
        with open(str(target[0]), 'w', encoding='utf-8') as f:
            f.write(target_name + '\n')
        return None

    stamp = os.path.join(out_dir, f'.{target_name}.stamp')
    target = env.Command(stamp, [], env.Action(_generate_docs, '[DOCS] 生成中…'))
    env.AlwaysBuild(target)
    return [target]
