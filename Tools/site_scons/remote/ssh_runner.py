# -*- coding: utf-8 -*-
"""SSH 远程执行器（命令行 ssh 实现，兼容 Windows 内置 OpenSSH）。

设计选择：默认走系统 ssh 命令，而非 paramiko —— 因为 paramiko 在 Windows
对 ssh-agent / key 管理支持不如系统 OpenSSH 完善。如需 paramiko，请把
本类替换为 ParamikoSshRunner。
"""
import os
import shlex
import subprocess
import sys
from .sync import push, pull


class SshRunner:
    def __init__(self, server_cfg: dict):
        self.cfg = server_cfg

    def _ssh_argv(self):
        argv = ['ssh', '-p', str(self.cfg.get('port', 22))]
        if self.cfg.get('auth') == 'key' and self.cfg.get('key_file'):
            argv += ['-i', os.path.expanduser(self.cfg['key_file'])]
        argv += ['-o', 'StrictHostKeyChecking=accept-new']
        argv.append(f'{self.cfg["user"]}@{self.cfg["host"]}')
        return argv

    def run_remote(self, shell_cmd: str) -> int:
        """单次远程命令执行。"""
        argv = self._ssh_argv() + [shell_cmd]
        return subprocess.call(argv)

    def run_build(self, scons_args: str, sync_cfg: dict,
                  local_root: str, pull_back: bool = True) -> int:
        host = self.cfg['host']
        remote_dir = self.cfg['remote_dir']

        # 1) 推送
        print(f'[REMOTE] ① 同步源码 → {host}:{remote_dir}')
        rc = push(local_root, self.cfg, sync_cfg)
        if rc != 0:
            return rc

        # 2) 远程执行 scons
        jobs = self.cfg.get('jobs', 8)
        remote_cmd = (f'cd {shlex.quote(remote_dir)} && '
                      f'scons -j{jobs} {scons_args}')
        print(f'[REMOTE] ② 远程执行：{remote_cmd}')
        rc = self.run_remote(remote_cmd)
        if rc != 0:
            print(f'[REMOTE] 构建失败 rc={rc}', file=sys.stderr)
            return rc

        # 3) 拉回产物
        if pull_back:
            print(f'[REMOTE] ③ 回传产物')
            rc = pull(local_root, self.cfg, sync_cfg)
        return rc
