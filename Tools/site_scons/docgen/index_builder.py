# -*- coding: utf-8 -*-
"""项目级模块索引 HTML 生成。"""
import datetime
import os


_HTML_TEMPLATE = """<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<title>{title}</title>
<style>
  body {{ font-family: "Microsoft YaHei", sans-serif; max-width: 1100px; margin: 30px auto; padding: 0 20px; }}
  h1 {{ color: #1a5490; border-bottom: 2px solid #1a5490; padding-bottom: 8px; }}
  table {{ border-collapse: collapse; width: 100%; margin: 20px 0; }}
  th {{ background: #1a5490; color: #fff; padding: 8px 12px; text-align: left; }}
  td {{ padding: 6px 12px; border-bottom: 1px solid #eee; }}
  .enabled {{ color: #27ae60; font-weight: 600; }}
  .disabled {{ color: #95a5a6; }}
  .meta {{ color: #555; font-size: 14px; }}
  pre.mermaid {{ background: #f8f9fa; padding: 12px; border-radius: 4px; overflow-x: auto; }}
</style>
</head>
<body>
<h1>{title}</h1>
<p class="meta">项目：<b>{project}</b>　|　生成时间：{ts}　|　启用 {n_on}/{n_total} 个模块</p>

<h2>模块清单</h2>
<table>
  <tr><th>#</th><th>模块</th><th>状态</th><th>接口文档</th></tr>
  {rows}
</table>

<h2>依赖关系图</h2>
<p>用 Mermaid 渲染（VSCode 装 Mermaid 插件、GitHub 原生支持，或访问 <a href="https://mermaid.live">mermaid.live</a> 粘贴预览）：</p>
<pre class="mermaid">{mermaid}</pre>
</body>
</html>
"""


def build_index_html(output_dir: str, title: str, project: str,
                     modules: list, all_bsw: list) -> str:
    ts = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    enabled_names = {m.name for m in modules}
    rows = []
    for i, m in enumerate(all_bsw, 1):
        on = m.name in enabled_names
        status_cls = 'enabled' if on else 'disabled'
        status_txt = '✓ enabled' if on else '— disabled'
        link = f'<a href="modules/{m.name}/interfaces.md">interfaces.md</a>' if on else '—'
        rows.append(
            f'<tr><td>{i}</td><td><b>{m.name}</b></td>'
            f'<td class="{status_cls}">{status_txt}</td>'
            f'<td>{link}</td></tr>'
        )

    # 读 mermaid 文件（若存在）
    mermaid_path = os.path.join(output_dir, 'graphs', 'dependency.mmd')
    mermaid = ''
    if os.path.isfile(mermaid_path):
        with open(mermaid_path, encoding='utf-8') as f:
            mermaid = f.read()
        # 去掉外层 ``` 包裹
        mermaid = mermaid.replace('```mermaid', '').replace('```', '').strip()

    html = _HTML_TEMPLATE.format(
        title    = title,
        project  = project,
        ts       = ts,
        n_on     = len(enabled_names),
        n_total  = len(all_bsw),
        rows     = '\n  '.join(rows),
        mermaid  = mermaid or '（暂无依赖图）',
    )
    out_file = os.path.join(output_dir, 'index.html')
    with open(out_file, 'w', encoding='utf-8') as f:
        f.write(html)
    return out_file
