#!/usr/bin/env python3
"""本地静态检查控制台 (check dashboard)。

作为 Tools/scripts/run_check.py 的图形入口: 在页面选择检查范围 (全项目 / 模块 /
文件 / 仅 git 改动)、勾选运行选项、保存配置并运行、打开 HTML 报告, 以及管理
misra.json 的 suppressPaths / deviations / ruleTexts。

用法:
    python Tools/scripts/check_dashboard.py --project Demo_Tc387 [--port 8975] [--no-browser]

普通 file:// 静态页面不能执行本机命令, 因此由本脚本起一个只监听 127.0.0.1 的
本地 Python Web 服务, 页面仅作为控制台; 不要把它作为远程服务暴露到网络。
配置保存在 Projects/<P>/Reports/check_dashboard_config.json。
"""
import argparse
import json
import os
import shutil
import subprocess
import sys
import threading
import webbrowser
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

SCRIPTS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(SCRIPTS, '..', '..'))
sys.path.insert(0, SCRIPTS)
sys.path.insert(0, os.path.join(ROOT, 'Tools'))
import run_check  # noqa: E402  复用 cppcheck 探测 / misra.json 与 build.yaml 加载


# ───────────────────────── 运行状态 (单并发) ─────────────────────────

class RunState:
    def __init__(self):
        self.lock = threading.Lock()
        self.lines = []
        self.rc = None
        self.running = False
        self.cmd = ''

    def start(self, cmd_list):
        with self.lock:
            if self.running:
                return False
            self.lines, self.rc, self.running = [], None, True
            self.cmd = ' '.join(cmd_list)
        threading.Thread(target=self._work, args=(cmd_list,), daemon=True).start()
        return True

    def _work(self, cmd_list):
        try:
            p = subprocess.Popen(cmd_list, cwd=ROOT, text=True, encoding='utf-8', errors='replace',
                                 stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            for line in p.stdout:
                with self.lock:
                    self.lines.append(line.rstrip('\n'))
            rc = p.wait()
        except Exception as exc:
            with self.lock:
                self.lines.append(f'[dashboard] 运行异常: {exc}')
            rc = -1
        with self.lock:
            self.rc, self.running = rc, False

    def snapshot(self, since):
        with self.lock:
            return {'running': self.running, 'rc': self.rc, 'cmd': self.cmd,
                    'lines': self.lines[since:], 'total': len(self.lines)}


RUN = RunState()


# ───────────────────────── 配置 <-> 命令行 ─────────────────────────

DEFAULT_CONFIG = {
    'scope': 'project',          # project | modules | paths | changed
    'modules': [], 'paths': [],
    'no_tests': True, 'html': True, 'open_report': False,
    'strict_misra': False, 'config_only': False,
    'jobs': 0, 'require_cppcheck': False,
}


def build_cmd(project, cfg):
    cmd = [sys.executable, os.path.join('Tools', 'scripts', 'run_check.py'), '--project', project]
    scope = cfg.get('scope', 'project')
    if scope == 'modules':
        for m in cfg.get('modules', []):
            cmd += ['--module', m]
    elif scope == 'paths':
        for p in cfg.get('paths', []):
            if p.strip():
                cmd += ['--path', p.strip()]
    elif scope == 'changed':
        cmd.append('--changed-only')
    for key, flag in (('no_tests', '--no-tests'), ('html', '--html'), ('open_report', '--open-report'),
                      ('strict_misra', '--strict-misra'), ('config_only', '--config-only'),
                      ('require_cppcheck', '--require-cppcheck')):
        if cfg.get(key):
            cmd.append(flag)
    jobs = int(cfg.get('jobs') or 0)
    if jobs > 0:
        cmd += ['--jobs', str(jobs)]
    return cmd


def config_file(project):
    return os.path.join(ROOT, 'Projects', project, 'Reports', 'check_dashboard_config.json')


def load_config(project):
    cfg = dict(DEFAULT_CONFIG)
    f = config_file(project)
    if os.path.exists(f):
        try:
            with open(f, encoding='utf-8') as fh:
                cfg.update(json.load(fh))
        except Exception:
            pass
    return cfg


def save_config(project, cfg):
    f = config_file(project)
    os.makedirs(os.path.dirname(f), exist_ok=True)
    with open(f, 'w', encoding='utf-8') as fh:
        json.dump(cfg, fh, ensure_ascii=False, indent=2)


# ───────────────────────── 状态查询 ─────────────────────────

def module_list(project):
    """build.yaml 显式清单 + 工作区自动发现, 合并去重 -> ['Layer/Mod', ...]。"""
    mods = set()
    cfg = run_check.load_build_config(project)
    chip = cfg.chip if cfg is not None else ''
    if cfg is not None:
        for layer, m in cfg.layers.items():
            for name in m:
                mods.add(f'{layer}/{name}')
    try:
        from site_scons.workspace import Workspace
        for layer, found in Workspace(ROOT, chip).discover().items():
            for name in found:
                mods.add(f'{layer}/{name}')
    except Exception:
        pass
    return sorted(mods)


def api_state(project):
    exe = run_check.find_cppcheck()
    misra, misra_file = run_check.load_misra(project)
    report = os.path.join(ROOT, 'Projects', project, 'Reports', 'static_check', 'index.html')
    return {
        'project': project,
        'cppcheck': {'path': exe or '', 'version': run_check.cppcheck_version(exe) if exe else ''},
        'modules': module_list(project),
        'misra': {'file': os.path.relpath(misra_file, ROOT),
                  'suppressPaths': misra.get('suppressPaths', []),
                  'deviations': misra.get('deviations', []),
                  'ruleTexts': misra.get('ruleTexts', '')},
        'config': load_config(project),
        'reportExists': os.path.exists(report),
    }


def save_misra(project, data):
    misra, misra_file = run_check.load_misra(project)
    shutil.copyfile(misra_file, misra_file + '.bak')
    if 'suppressPaths' in data:
        misra['suppressPaths'] = [p for p in data['suppressPaths'] if p.strip()]
    if 'deviations' in data:
        misra['deviations'] = [d for d in data['deviations'] if d.get('rule', '').strip()]
    if 'ruleTexts' in data:
        if data['ruleTexts'].strip():
            misra['ruleTexts'] = data['ruleTexts'].strip()
        else:
            misra.pop('ruleTexts', None)
    with open(misra_file, 'w', encoding='utf-8') as fh:
        json.dump(misra, fh, ensure_ascii=False, indent=2)
        fh.write('\n')
    return os.path.relpath(misra_file, ROOT)


# ───────────────────────── 页面 (自包含, 无外网依赖) ─────────────────────────

PAGE = """<!DOCTYPE html>
<html lang="zh"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>OpenAutosar 静态检查控制台</title>
<style>
 :root { --bg:#fff; --fg:#1a202c; --line:#cbd5e0; --panel:#f7fafc; --accent:#2b6cb0; --ok:#2f855a; --bad:#c53030; }
 @media (prefers-color-scheme: dark) {
   :root { --bg:#1a202c; --fg:#e2e8f0; --line:#4a5568; --panel:#2d3748; --accent:#63b3ed; --ok:#68d391; --bad:#fc8181; }
 }
 body { font-family: system-ui,"Segoe UI",sans-serif; margin:1.5rem auto; max-width:64rem; padding:0 1rem;
        background:var(--bg); color:var(--fg); }
 h1 { font-size:1.3rem; } h2 { font-size:1rem; margin:.2rem 0 .6rem; }
 fieldset { border:1px solid var(--line); border-radius:.5rem; margin:0 0 1rem; padding: .8rem 1rem; background:var(--panel); }
 legend { font-weight:600; padding:0 .4rem; }
 label { margin-right:1.2rem; white-space:nowrap; }
 select,input[type=text],input[type=number],textarea { background:var(--bg); color:var(--fg);
   border:1px solid var(--line); border-radius:.3rem; padding:.25rem .4rem; font: inherit; }
 select[multiple] { min-width:18rem; min-height:9rem; }
 textarea { width:100%; box-sizing:border-box; font-family:ui-monospace,Consolas,monospace; font-size:.85rem; }
 button { background:var(--accent); color:#fff; border:0; border-radius:.4rem; padding:.45rem 1rem;
          font:inherit; cursor:pointer; margin-right:.6rem; }
 button.sec { background:var(--panel); color:var(--fg); border:1px solid var(--line); }
 button:disabled { opacity:.5; cursor:default; }
 pre { background:var(--panel); border:1px solid var(--line); border-radius:.4rem; padding:.7rem;
       overflow-x:auto; font-size:.82rem; max-height:22rem; overflow-y:auto; }
 code { font-family:ui-monospace,Consolas,monospace; }
 .ok { color:var(--ok); } .bad { color:var(--bad); }
 .hint { font-size:.82rem; opacity:.75; }
 table { border-collapse:collapse; width:100%; font-size:.85rem; }
 th,td { border:1px solid var(--line); padding:.25rem .4rem; text-align:left; }
 td input { width:95%; }
 .row { margin:.4rem 0; }
</style></head><body>
<h1>静态检查控制台 <span id="proj"></span></h1>
<p class="hint" id="toolinfo"></p>

<fieldset><legend>检查范围</legend>
 <div class="row">
  <label><input type="radio" name="scope" value="project"> 全项目 (misra.json include)</label>
  <label><input type="radio" name="scope" value="modules"> 指定模块</label>
  <label><input type="radio" name="scope" value="paths"> 自定义路径</label>
  <label><input type="radio" name="scope" value="changed"> 仅 git 改动的 .c/.h</label>
 </div>
 <div class="row" id="modrow" hidden>
  <select id="modules" multiple></select>
  <div class="hint">Ctrl/Cmd 多选; 清单 = build.yaml 显式模块 + 工作区自动发现</div>
 </div>
 <div class="row" id="pathrow" hidden>
  <textarea id="paths" rows="3" placeholder="每行一个文件或目录, 如 CDD/CddPwm/src/CddPwm.c"></textarea>
 </div>
</fieldset>

<fieldset><legend>运行选项</legend>
 <label><input type="checkbox" id="no_tests"> 不跑单测</label>
 <label><input type="checkbox" id="html"> 生成 HTML 报告</label>
 <label><input type="checkbox" id="open_report"> 运行后打开报告</label>
 <label><input type="checkbox" id="strict_misra"> MISRA 严格门禁</label>
 <label><input type="checkbox" id="config_only"> 只生成有效配置</label>
 <label><input type="checkbox" id="require_cppcheck"> 缺 cppcheck 视为失败</label>
 <label>并行度 <input type="number" id="jobs" min="0" max="64" style="width:4rem"> <span class="hint">0=自动</span></label>
</fieldset>

<fieldset><legend>MISRA 配置 (<code id="misrafile"></code>)</legend>
 <div class="row"><label>规则正文文件 (rule-texts, 需自备授权文本):
  <input type="text" id="ruleTexts" size="46" placeholder="如 Tools/misra_rule_texts.txt"></label></div>
 <div class="row">排除路径 (suppressPaths, 每行一个, 支持 * 与 /**):
  <textarea id="suppress" rows="3"></textarea></div>
 <div class="row">偏离 (deviations):
  <table id="devtable"><thead><tr><th style="width:6rem">规则</th><th>理由</th><th>范围 scope (逗号分隔)</th><th style="width:3rem"></th></tr></thead><tbody></tbody></table>
  <button class="sec" type="button" onclick="addDev()">+ 添加偏离</button>
  <button class="sec" type="button" onclick="saveMisra()">保存 MISRA 配置</button>
  <span id="misramsg" class="hint"></span>
 </div>
</fieldset>

<fieldset><legend>执行</legend>
 <div class="row">命令预览: <pre id="preview" style="max-height:6rem"></pre></div>
 <button type="button" onclick="save(false)">保存配置</button>
 <button type="button" id="runbtn" onclick="save(true)">保存并运行</button>
 <button type="button" class="sec" id="reportbtn" onclick="window.open('/report')">打开报告</button>
 <span id="status"></span>
 <pre id="output" hidden></pre>
</fieldset>

<p class="hint">本页面仅监听 127.0.0.1, 供开发机本地(含离线)静态检查使用; 请勿暴露到网络。</p>

<script>
let SEEN = 0, POLL = null;

function cfgFromUI() {
  return {
    scope: document.querySelector('input[name=scope]:checked').value,
    modules: Array.from(document.getElementById('modules').selectedOptions).map(o => o.value),
    paths: document.getElementById('paths').value.split('\\n').map(s => s.trim()).filter(Boolean),
    no_tests: no_tests.checked, html: html.checked, open_report: open_report.checked,
    strict_misra: strict_misra.checked, config_only: config_only.checked,
    require_cppcheck: require_cppcheck.checked, jobs: Number(jobs.value || 0),
  };
}

function applyCfg(c) {
  document.querySelector(`input[name=scope][value=${c.scope || 'project'}]`).checked = true;
  for (const o of document.getElementById('modules').options) o.selected = (c.modules || []).includes(o.value);
  document.getElementById('paths').value = (c.paths || []).join('\\n');
  for (const k of ['no_tests','html','open_report','strict_misra','config_only','require_cppcheck'])
    document.getElementById(k).checked = !!c[k];
  jobs.value = c.jobs || 0;
  scopeUI();
}

function scopeUI() {
  const s = document.querySelector('input[name=scope]:checked').value;
  document.getElementById('modrow').hidden = s !== 'modules';
  document.getElementById('pathrow').hidden = s !== 'paths';
  preview();
}

async function preview() {
  const r = await fetch('/api/preview', {method:'POST', body: JSON.stringify(cfgFromUI())});
  document.getElementById('preview').textContent = (await r.json()).preview;
}

async function save(run) {
  const r = await fetch(run ? '/api/run' : '/api/save', {method:'POST', body: JSON.stringify(cfgFromUI())});
  const d = await r.json();
  document.getElementById('preview').textContent = d.preview || '';
  if (run) {
    if (!d.started) { setStatus('已有检查在运行', 'bad'); return; }
    SEEN = 0; document.getElementById('output').hidden = false;
    document.getElementById('output').textContent = '';
    document.getElementById('runbtn').disabled = true;
    setStatus('运行中 …');
    POLL = setInterval(pollOutput, 800);
  } else setStatus('配置已保存', 'ok');
}

async function pollOutput() {
  const d = await (await fetch('/api/output?since=' + SEEN)).json();
  const out = document.getElementById('output');
  if (d.lines.length) { out.textContent += d.lines.join('\\n') + '\\n'; out.scrollTop = out.scrollHeight; }
  SEEN = d.total;
  if (!d.running && d.rc !== null) {
    clearInterval(POLL);
    document.getElementById('runbtn').disabled = false;
    setStatus(d.rc === 0 ? '完成 ✔ (rc=0)' : `失败 ✖ (rc=${d.rc})`, d.rc === 0 ? 'ok' : 'bad');
    refreshState();
  }
}

function setStatus(t, cls) { const s = document.getElementById('status'); s.textContent = t; s.className = cls || ''; }

function devRow(d) {
  const tr = document.createElement('tr');
  tr.innerHTML = `<td><input value="${d.rule || ''}"></td><td><input value="${d.reason || ''}"></td>
    <td><input value="${(d.scope || []).join(', ')}"></td>
    <td><button class="sec" type="button" onclick="this.closest('tr').remove()">✕</button></td>`;
  return tr;
}
function addDev() { document.querySelector('#devtable tbody').appendChild(devRow({})); }

async function saveMisra() {
  const devs = Array.from(document.querySelectorAll('#devtable tbody tr')).map(tr => {
    const [rule, reason, scope] = Array.from(tr.querySelectorAll('input')).map(i => i.value);
    return {rule: rule.trim(), reason: reason.trim(), scope: scope.split(',').map(s => s.trim()).filter(Boolean)};
  });
  const body = {suppressPaths: document.getElementById('suppress').value.split('\\n').map(s => s.trim()).filter(Boolean),
                deviations: devs, ruleTexts: document.getElementById('ruleTexts').value};
  const d = await (await fetch('/api/misra', {method:'POST', body: JSON.stringify(body)})).json();
  document.getElementById('misramsg').textContent = `已写入 ${d.file} (备份 .bak)`;
}

async function refreshState() {
  const s = await (await fetch('/api/state')).json();
  document.getElementById('proj').textContent = '· ' + s.project;
  document.getElementById('toolinfo').innerHTML = s.cppcheck.path
    ? `cppcheck: <span class="ok">${s.cppcheck.version}</span> (${s.cppcheck.path})`
    : '<span class="bad">未找到 cppcheck — Windows 用离线包 (setup_env.py --offline), Linux 需 apt install cppcheck</span>';
  const sel = document.getElementById('modules'), cur = new Set((cfgFromUI().modules) || []);
  if (!sel.options.length) {
    for (const m of s.modules) { const o = document.createElement('option'); o.value = o.textContent = m; sel.appendChild(o); }
  }
  document.getElementById('misrafile').textContent = s.misra.file;
  document.getElementById('suppress').value = s.misra.suppressPaths.join('\\n');
  document.getElementById('ruleTexts').value = s.misra.ruleTexts || '';
  const tb = document.querySelector('#devtable tbody'); tb.innerHTML = '';
  for (const d of s.misra.deviations) tb.appendChild(devRow(d));
  document.getElementById('reportbtn').disabled = !s.reportExists;
  return s;
}

(async () => {
  const s = await refreshState();
  applyCfg(s.config);
  for (const el of document.querySelectorAll('input,select,textarea')) el.addEventListener('change', preview);
  for (const r of document.querySelectorAll('input[name=scope]')) r.addEventListener('change', scopeUI);
  preview();
})();
</script>
</body></html>
"""


# ───────────────────────── HTTP 服务 ─────────────────────────

def make_handler(project):
    class Handler(BaseHTTPRequestHandler):
        def _send(self, code, body, ctype='application/json; charset=utf-8'):
            data = body if isinstance(body, bytes) else json.dumps(body, ensure_ascii=False).encode('utf-8')
            self.send_response(code)
            self.send_header('Content-Type', ctype)
            self.send_header('Content-Length', str(len(data)))
            self.end_headers()
            self.wfile.write(data)

        def _body(self):
            n = int(self.headers.get('Content-Length', 0))
            return json.loads(self.rfile.read(n).decode('utf-8')) if n else {}

        def log_message(self, fmt, *args):
            pass  # 安静; 需要时可打印 self.path

        def do_GET(self):
            path = self.path.split('?')[0]
            if path == '/':
                self._send(200, PAGE.encode('utf-8'), 'text/html; charset=utf-8')
            elif path == '/api/state':
                self._send(200, api_state(project))
            elif path == '/api/output':
                since = 0
                if 'since=' in self.path:
                    try:
                        since = int(self.path.split('since=')[1].split('&')[0])
                    except ValueError:
                        pass
                self._send(200, RUN.snapshot(since))
            elif path == '/report':
                report = os.path.join(ROOT, 'Projects', project, 'Reports', 'static_check', 'index.html')
                if os.path.exists(report):
                    with open(report, 'rb') as f:
                        self._send(200, f.read(), 'text/html; charset=utf-8')
                else:
                    self._send(404, {'error': '报告不存在, 请先勾选"生成 HTML 报告"运行一次'})
            else:
                self._send(404, {'error': 'not found'})

        def do_POST(self):
            try:
                cfg = self._body()
            except Exception:
                self._send(400, {'error': 'bad json'})
                return
            if self.path == '/api/preview':
                self._send(200, {'preview': ' '.join(build_cmd(project, cfg))})
            elif self.path == '/api/save':
                save_config(project, cfg)
                self._send(200, {'saved': True, 'preview': ' '.join(build_cmd(project, cfg))})
            elif self.path == '/api/run':
                save_config(project, cfg)
                started = RUN.start(build_cmd(project, cfg))
                self._send(200, {'started': started, 'preview': ' '.join(build_cmd(project, cfg))})
            elif self.path == '/api/misra':
                self._send(200, {'file': save_misra(project, cfg)})
            else:
                self._send(404, {'error': 'not found'})

    return Handler


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--project', default=os.environ.get('PROJECT', 'Demo_Tc387'))
    ap.add_argument('--port', type=int, default=8975)
    ap.add_argument('--no-browser', action='store_true', help='不自动打开浏览器')
    a = ap.parse_args()

    if not os.path.isdir(os.path.join(ROOT, 'Projects', a.project)):
        raise SystemExit(f'[dashboard] fatal: 项目不存在: Projects/{a.project}')

    srv = ThreadingHTTPServer(('127.0.0.1', a.port), make_handler(a.project))
    url = f'http://127.0.0.1:{srv.server_address[1]}/'
    print(f'[dashboard] 静态检查控制台: {url}  (项目 {a.project}, Ctrl+C 退出)')
    if not a.no_browser:
        webbrowser.open(url)
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        print('\n[dashboard] bye')
    return 0


if __name__ == '__main__':
    sys.exit(main())
