# Tools/vendor — 随仓库提交的离线工具/依赖 (Windows x86_64)

目的: 让**网络受限**的机器克隆仓库后无需任何外网下载即可搭好构建环境。
`Tools/fetch_tools.py` 与 `Tools/setup_env.py` **离线优先**使用本目录;仅当本目录
缺少对应平台归档时才回退联网下载。

> 平台范围: **Windows x86_64**(开发机/Vector 工具链所在平台)。
> Linux CI 走系统 `apt`(cppcheck)+ 联网/系统 cmake,无需这些二进制。

## 内容

| 子目录 | 文件 | 用途 | 解压去向 |
|--------|------|------|----------|
| `wheels/` | `scons-4.10.1-*.whl`、`PyYAML-6.0.1-cp39..cp312-win_amd64.whl` | 离线 `pip install`（`--no-index --find-links`） | venv `Tools/.venv/` |
| `archives/` | `cmake-3.28.6-windows-x86_64.zip` | 单元测试构建 | `Tools/.toolcache/cmake-3.28.6-windows-x86_64/` |
| `archives/` | `googletest-1.14.0.tar.gz` | 离线单测（FetchContent 不联网） | `Tools/.toolcache/googletest-1.14.0/` |
| `installers/` | `cppcheck-2.13.0-x64-Setup.msi` | `scons check` 静态/MISRA | `Tools/.toolcache/cppcheck/`（`msiexec /a` 解压为便携版） |

版本与 **SHA-256** 见 [`manifest.json`](manifest.json)（`cmake` 已对 Kitware 官方校验和核对）。

## 一键使用

```bat
python Tools\setup_env.py
```

它会:① 用 `wheels/` 离线建 venv;② 调 `fetch_tools.py` 把 `archives/`、`installers/`
解压到 `Tools\.toolcache\`(gitignore);全程不联网。`run_gtest.py` / `run_check.py`
会自动优先用 `.toolcache` 里的 cmake / googletest / cppcheck。

## 不在此处(无法/不宜 vendoring)

| 工具 | 原因 | 获取 |
|------|------|------|
| `gcc/g++` 或 MSVC (host 编译器) | 体积大 | 系统安装(MinGW/MSVC),单测/`TOOLCHAIN=gcc` 用 |
| TriCore `tricore-gcc`/TASKING | 厂商授权,不可再分发 | 官网安装,`HIGHTEC_BIN`/`TASKING_BIN` 或 PATH |
| `qemu-system-tricore` | 需自建/体积 | 自托管 runner 或本机装好 |
| Python 解释器 | 系统基础 | 装原生 Windows Python ≥3.9(本 wheel 覆盖 cp39–cp312) |

## 升级某工具

1. 换 `archives/` 或 `installers/` 下的新版本文件、改 `fetch_tools.py` 顶部版本常量;
2. 重新生成 `manifest.json` 的 sha256;
3. `python Tools\fetch_tools.py --force` 重解压并重测。
