# -*- coding: utf-8 -*-
"""rsync 同步层。"""
import os
import subprocess
import sys


def _posix(p):
    """Windows 路径转 cygwin 风格（rsync 在 Windows 上要这种形式）。

    D:\foo\bar  →  /cygdrive/d/foo/bar
    """
    p = p.replace('\\', '/')
    if len(p) >= 2 and p[1] == ':':
        return f'/cygdrive/{p[0].lower()}{p[2:]}'
    return p


def _excludes(excludes):
    out = []
    for e in (excludes or []):
        out += ['--exclude', e]
    return out


def _ssh_transport(server):
    argv = ['ssh', '-p', str(server.get('port', 22))]
    if server.get('auth') == 'key' and server.get('key_file'):
        argv += ['-i', os.path.expanduser(server['key_file'])]
    return ' '.join(f'"{x}"' if ' ' in x else x for x in argv)


def push(local_root: str, server: dict, sync_cfg: dict) -> int:
    """本地 → 远程。"""
    remote = f'{server["user"]}@{server["host"]}:{server["remote_dir"]}/'
    src = _posix(local_root) + '/'
    cmd = ['rsync', '-avz', '--delete', '-e', _ssh_transport(server)]
    cmd += _excludes(sync_cfg.get('excludes'))
    cmd += [src, remote]
    try:
        return subprocess.call(cmd)
    except FileNotFoundError:
        print('[REMOTE] 未找到 rsync 命令。Windows：choco install rsync；'
              'Linux：apt install rsync', file=sys.stderr)
        return 127


def pull(local_root: str, server: dict, sync_cfg: dict) -> int:
    """远程 → 本地。"""
    remote_base = f'{server["user"]}@{server["host"]}:{server["remote_dir"]}/'
    rc_all = 0
    for path in sync_cfg.get('pull_back', []) or []:
        local_path = _posix(os.path.join(local_root, path))
        os.makedirs(os.path.dirname(local_path.replace('/cygdrive/', ''))
                    if local_path.startswith('/cygdrive/') else
                    os.path.dirname(local_path), exist_ok=True)
        cmd = ['rsync', '-avz', '-e', _ssh_transport(server), remote_base + path, local_path]
        rc = subprocess.call(cmd)
        rc_all = rc_all or rc
    return rc_all
