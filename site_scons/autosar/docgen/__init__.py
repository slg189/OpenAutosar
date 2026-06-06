# -*- coding: utf-8 -*-
"""文档生成入口。"""
import os
from .interface_extractor import extract_module_interfaces, render_markdown
from .dependency_grapher import render_mermaid, infer_dependencies
from .doxygen_runner import run_doxygen
from .index_builder import build_index_html


def build_docs(env, cfg, enabled_modules, target_name='doc', include_site=True):
    """汇总入口。返回 SCons Target 列表。

    Args:
        env: SCons Environment
        cfg: BuildConfig
        enabled_modules: List[ModuleEntry]
    """
    targets = []
    out_dir = cfg.docs.output_dir or os.path.join(env['ROOT_DIR'], 'docs', 'api')
    os.makedirs(out_dir, exist_ok=True)
    os.makedirs(os.path.join(out_dir, 'modules'), exist_ok=True)
    os.makedirs(os.path.join(out_dir, 'graphs'),  exist_ok=True)

    bsw_dir = env['BSW_DIR']

    def _generate_docs(target, source, env_):
        # 1) 接口抽取（每模块一份 markdown）
        if cfg.docs.interface_enabled:
            for m in enabled_modules:
                mod_dir = os.path.join(bsw_dir, m.name)
                if not os.path.isdir(mod_dir):
                    continue
                ifaces = extract_module_interfaces(mod_dir)
                md = render_markdown(m.name, ifaces)
                mod_out = os.path.join(out_dir, 'modules', m.name)
                os.makedirs(mod_out, exist_ok=True)
                with open(os.path.join(mod_out, 'interfaces.md'),
                          'w', encoding='utf-8') as f:
                    f.write(md)

        # 2) 依赖图
        if include_site and cfg.docs.dep_graph_enabled:
            deps = infer_dependencies(bsw_dir, [m.name for m in enabled_modules])
            mermaid = render_mermaid(
                [{'name': m.name, 'enabled': m.enabled}
                 for m in enabled_modules],
                deps,
            )
            with open(os.path.join(out_dir, 'graphs', 'dependency.mmd'),
                      'w', encoding='utf-8') as f:
                f.write(mermaid)

        # 3) Doxygen（可选）
        if cfg.docs.doxygen_enabled and cfg.docs.doxygen_config:
            run_doxygen(
                config_template = cfg.docs.doxygen_config,
                modules         = enabled_modules,
                bsw_dir         = bsw_dir,
                output_dir      = out_dir,
            )

        # 4) 索引页
        if include_site and cfg.docs.module_index:
            build_index_html(
                output_dir = out_dir,
                title      = cfg.docs.index_title or f'{cfg.project_name} 模块清单',
                project    = cfg.project_name,
                modules    = enabled_modules,
                all_bsw    = cfg.bsw_modules,
            )

        with open(str(target[0]), 'w', encoding='utf-8') as f:
            f.write(target_name + '\n')
        return None

    stamp = os.path.join(out_dir, f'.{target_name}.stamp')
    target = env.Command(stamp, [], env.Action(_generate_docs, '[DOCS] 生成中…'))
    env.AlwaysBuild(target)
    targets.append(target)
    return targets
