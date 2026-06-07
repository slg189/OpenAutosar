#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
OpenAutosar 顶层构建入口。

用法：
    scons                                  # 本地构建 (默认 Projects/Demo_Tc387)
    scons TOOLCHAIN=hightec PLATFORM=AURIX2G
    scons PROJECT=Projects/Demo_Tc387 CONFIG=build.yaml
    scons only=Adc,CddPwm                  # 仅构建指定模块 (名或 leaf)
    scons skip=EnergyManagement            # 跳过指定模块
    scons layers=MCAL,ASW                  # 仅构建指定层
    scons list                             # 打印模块/mode/来源
    scons gen                              # 只跑代码生成
    scons doc                              # 只生成文档
    scons release                          # 源码模块释放到 *_Libs
    scons check                            # 静态检查 + 单元测试入口
    scons --remote=build_server_01         # 远程构建

说明：这是 v3 框架(YAML 驱动)的唯一顶层入口；旧的 per-project 构建
(Projects/<P>/SConstruct + Tools/Scons/build_helpers.py + module_config.py) 已退役。
用户手册见 Projects/Demo_Tc387/Docs/USER_MANUAL.html。
"""
import os
import sys

# 把 Tools 加进 Python 搜索路径, 框架为其下的 site_scons 包 (Tools/site_scons/)。
# 注: 这里的 site_scons 是普通 Python 包(非 SCons 根目录魔法位置), 用包名隔离避免
#     platform/codegen 等模块名污染/遮蔽 (如 stdlib 的 platform)。
ROOT = Dir('#').abspath
sys.path.insert(0, os.path.join(ROOT, 'Tools'))

import yaml
from site_scons.toolchain import load_toolchain
from site_scons.platform  import load_platform
from site_scons.remote    import dispatch_if_remote

AddOption('--remote',
          dest='remote',
          nargs=1,
          type='string',
          default=None,
          help='Run build on configured remote server name, or "auto".')
AddOption('--no-pull',
          dest='no_pull',
          action='store_true',
          default=False,
          help='Do not pull remote build outputs back after remote build.')
AddOption('--skip-gen',
          dest='skip_gen',
          action='store_true',
          default=False,
          help='Skip configured code generators before compilation.')

# ── 1. 命令行变量声明 ──────────────────────────────────────
vars_ = Variables(None, ARGUMENTS)
vars_.AddVariables(
    ('PROJECT',    '项目目录（相对于仓库根）',  'Projects/Demo_Tc387'),
    ('CONFIG',     'build.yaml 文件名',         'build.yaml'),
    ('TOOLCHAIN',  '工具链（覆盖 YAML）',         ''),
    ('PLATFORM',   '平台（覆盖 YAML）',           ''),
    ('BUILD_TYPE', '构建类型 Debug/Release',      ''),
    ('JOBS',       '并行编译任务数',              '8'),
    ('STRICT',     '严格模式: 1 时 -Werror (告警即错)', '0'),
)

env = Environment(variables=vars_, ENV=os.environ)
Help(vars_.GenerateHelpText(env))

def _normalize_project(project):
    project = project.replace('\\', '/').strip('/')
    if project.startswith('Projects/'):
        return project
    return 'Projects/' + project

env['PROJECT'] = _normalize_project(env['PROJECT'])

# ── 2. 远程分发：如指定 --remote 则同步源码并远程执行后退出 ─────
remote_target = GetOption('remote') or ARGUMENTS.get('REMOTE')
if remote_target is not None:
    project_dir = os.path.join(ROOT, env['PROJECT'])
    yaml_path = os.path.join(project_dir, env['CONFIG'])
    rc = dispatch_if_remote(
        target_name  = remote_target or 'default',
        yaml_path    = yaml_path,
        local_root   = ROOT,
        forward_args = {k: v for k, v in ARGUMENTS.items() if k not in ('REMOTE', 'remote')},
        pull_back    = not GetOption('no_pull'),
    )
    Exit(rc)

# ── 3. 预读 build.yaml，让命令行参数能覆盖 YAML 默认值 ───────
yaml_file = os.path.join(ROOT, env['PROJECT'], env['CONFIG'])
if not os.path.exists(yaml_file):
    print(f'[FATAL] 配置文件不存在：{yaml_file}', file=sys.stderr)
    Exit(1)

with open(yaml_file, encoding='utf-8') as f:
    pre = yaml.safe_load(f)
if not isinstance(pre, dict):
    print(f'[FATAL] 配置文件必须是 YAML mapping：{yaml_file}', file=sys.stderr)
    Exit(1)
if 'target' not in pre:
    print(f'[FATAL] 配置缺少 target 节点：{yaml_file}', file=sys.stderr)
    Exit(1)

env['TOOLCHAIN']  = env['TOOLCHAIN']  or pre['target']['toolchain']
env['PLATFORM']   = env['PLATFORM']   or pre['target']['platform']
env['BUILD_TYPE'] = env['BUILD_TYPE'] or pre['target'].get('build_type', 'Debug')

print(f'[INFO] toolchain  = {env["TOOLCHAIN"]}')
print(f'[INFO] platform   = {env["PLATFORM"]}')
print(f'[INFO] build_type = {env["BUILD_TYPE"]}')

# ── 4. 加载工具链 + 平台 ────────────────────────────────────
env = load_toolchain(env, env['TOOLCHAIN'])
env = load_platform(env, env['PLATFORM'])

# ── 5. 路径变量（暴露给所有 SConscript 用）────────────────────
env['ROOT_DIR']    = ROOT
env['BSW_DIR']     = os.path.join(ROOT, 'BSW')
env['MCAL_DIR']    = os.path.join(ROOT, 'MCAL')
env['COMP_DIR']    = os.path.join(ROOT, 'Components')
env['PROJECT_DIR'] = os.path.join(ROOT, env['PROJECT'])
env['GEN_DIR']     = os.path.join(env['PROJECT_DIR'], 'Gen')
env['OBJ_DIR']     = os.path.join(env['PROJECT_DIR'], 'Obj')
env['OUT_DIR']     = os.path.join(env['PROJECT_DIR'], 'Out')
env['DOCS_DIR']    = os.path.join(env['PROJECT_DIR'], 'Docs', 'api')

# 确保输出目录存在
for d in (env['OBJ_DIR'], env['OUT_DIR']):
    os.makedirs(d, exist_ok=True)

# 并行度
try:
    SetOption('num_jobs', int(env['JOBS']))
except (TypeError, ValueError):
    pass

# 让 SCons 用 MD5+timestamp 作为依赖判定（更精确）
env.Decider('MD5-timestamp')

# 导出 env 供子 SConscript 使用
Export('env')

# ── 6. 进入项目 SConscript ─────────────────────────────────
project_sconscript = os.path.join(env['PROJECT_DIR'], 'SConscript')
if not os.path.exists(project_sconscript):
    print(f'[FATAL] 项目 SConscript 不存在：{project_sconscript}', file=sys.stderr)
    Exit(1)

SConscript(
    project_sconscript,
    variant_dir = env['OBJ_DIR'],
    duplicate   = 0,
)
