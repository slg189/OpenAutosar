#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""独立的远程构建入口（不通过 SCons，也可单独使用）。

用法：
    python scripts/remote_build.py                          # 用默认 server
    python scripts/remote_build.py --server build_server_01
    python scripts/remote_build.py --auto                   # 自动调度
    python scripts/remote_build.py --extra "BUILD_TYPE=Release -j32"
"""
import argparse
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..'))
sys.path.insert(0, os.path.join(ROOT, 'site_scons'))

from autosar.remote import dispatch_if_remote


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--project', default='Projects/DemoProject')
    ap.add_argument('--config',  default='build.yaml')
    ap.add_argument('--server',  default='', help='指定服务器名')
    ap.add_argument('--auto',    action='store_true', help='自动调度')
    ap.add_argument('--no-pull', action='store_true', help='不回传产物')
    ap.add_argument('--extra',   default='', help='附加给 scons 的参数')
    args = ap.parse_args()

    target = 'auto' if args.auto else (args.server or 'default')
    yaml_path = os.path.join(ROOT, args.project, args.config)

    forward = {}
    for tok in args.extra.split():
        if '=' in tok:
            k, v = tok.split('=', 1)
            forward[k] = v

    rc = dispatch_if_remote(
        target_name  = target,
        yaml_path    = yaml_path,
        local_root   = ROOT,
        forward_args = forward,
        pull_back    = not args.no_pull,
    )
    sys.exit(rc)


if __name__ == '__main__':
    main()
