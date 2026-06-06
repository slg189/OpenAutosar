# -*- coding: utf-8 -*-
"""服务器调度策略。"""
import subprocess


def pick_server(servers: list, capability: str = '') -> dict:
    """从候选服务器中按能力 + 负载选一台。

    简化策略：
      1. 过滤出 capabilities 中含目标 toolchain 的
      2. ssh 探测 uptime，挑负载最小的
      3. 探测失败则取列表中的第一个
    """
    candidates = [s for s in servers
                  if (not capability) or capability in (s.get('capabilities') or [])]
    if not candidates:
        candidates = list(servers)

    best = candidates[0]
    best_load = float('inf')
    for s in candidates:
        load = _probe_load(s)
        if load is not None and load < best_load:
            best_load = load
            best = s
    return best


def _probe_load(server: dict):
    """ssh 探测 1 分钟平均 load。失败返回 None。"""
    cmd = [
        'ssh',
        '-o', 'BatchMode=yes',
        '-o', 'ConnectTimeout=3',
        '-p', str(server.get('port', 22)),
        f'{server["user"]}@{server["host"]}',
        "uptime | awk -F'load average:' '{print $2}' | awk -F',' '{print $1}'",
    ]
    try:
        out = subprocess.check_output(cmd, timeout=5, text=True)
        return float(out.strip())
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired,
            FileNotFoundError, ValueError):
        return None
