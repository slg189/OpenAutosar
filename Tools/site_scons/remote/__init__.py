# -*- coding: utf-8 -*-
"""远程构建入口。"""
import os
import sys
import yaml

from .ssh_runner import SshRunner
from .server_pool import pick_server


def dispatch_if_remote(target_name: str, yaml_path: str,
                       local_root: str, forward_args: dict,
                       pull_back: bool = True) -> int:
    """如果指定了 --remote，则同步源码 → 远程执行 → 拉回产物 → 返回 rc。

    target_name: 'default' / 'auto' / 具体 server 名
    """
    with open(yaml_path, encoding='utf-8') as f:
        cfg = yaml.safe_load(f)
    remote_cfg = cfg.get('remote', {}) or {}
    if not remote_cfg.get('enabled', False):
        print('[REMOTE] build.yaml 中 remote.enabled = false，远程构建被禁用',
              file=sys.stderr)
        return 1

    servers = remote_cfg.get('servers', []) or []
    if not servers:
        print('[REMOTE] build.yaml 中 remote.servers 为空', file=sys.stderr)
        return 1

    # 选择服务器
    if target_name in ('', 'default'):
        target_name = remote_cfg.get('default_target', servers[0]['name'])
    if target_name == 'auto':
        toolchain = forward_args.get('TOOLCHAIN') \
            or cfg.get('target', {}).get('toolchain', '')
        server = pick_server(servers, capability=toolchain)
    else:
        server = next((s for s in servers if s['name'] == target_name), None)

    if not server:
        print(f'[REMOTE] 未找到名为 "{target_name}" 的服务器', file=sys.stderr)
        return 1

    print(f'[REMOTE] 选定服务器：{server["name"]} ({server["host"]})')

    sync_cfg = remote_cfg.get('sync', {}) or {}
    runner = SshRunner(server)

    scons_args = ' '.join(f'{k}={v}' for k, v in forward_args.items())
    return runner.run_build(
        scons_args  = scons_args,
        sync_cfg    = sync_cfg,
        local_root  = local_root,
        pull_back   = pull_back,
    )
