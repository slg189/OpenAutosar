# 构建环境 (跨机器一致)

目标: 把构建所需工具尽量**收进 `Tools/`、固定版本**，减少“不同电脑环境不一致”导致的问题。

> **离线优先 (offline-first)**：可再分发的工具/依赖已**随仓库提交**到
> [`Tools/vendor/`](vendor/README.md)（Windows x86_64：cmake / googletest / cppcheck / scons+PyYAML wheel）。
> 网络受限的机器克隆后直接 `python Tools/setup_env.py` 即可，**全程不联网**；
> `setup_env`/`fetch_tools` 仅在 `vendor/` 缺该平台归档时才回退下载。

## 一键搭建

```bash
python Tools/setup_env.py          # ① 检查/版本校验 ② 离线优先把 vendor/ 工具解压到 .toolcache ③ 用 vendor/wheels 离线建 venv
python Tools/setup_env.py --check  # 只检查外部工具 + 版本硬校验 (cmake>=3.16)
python Tools/setup_env.py --no-fetch  # 跳过工具准备 (只建 venv)
python Tools/fetch_tools.py        # 单独: 把 vendor/ 的工具(或回退下载)解压到 Tools/.toolcache (gitignore)
```

搭好后（依赖与系统 Python 隔离）：

```bash
# Linux/mac
source Tools/.venv/bin/activate && scons
# Windows
Tools\.venv\Scripts\activate && scons
# 或不激活, 直接用 venv 里的 scons:
Tools/.venv/bin/scons            # Windows: Tools\.venv\Scripts\scons.exe
```

## 依赖清单

### A. Python 依赖 —— 已收进仓库、固定版本
| 项 | 位置 / 版本 | 说明 |
|----|------|------|
| Python | 系统 `python3` ≥ 3.9（实测 3.11） | venv 基于它创建 |
| 依赖清单 | **`Tools/requirements.txt`**（`scons==4.10.1`、`PyYAML==6.0.1`，固定） | venv 与 CI 都用它 |
| 隔离环境 | **`Tools/.venv/`**（`setup_env.py` 生成，已 gitignore） | 不受系统 SCons/PyYAML 版本影响 |
| 构建框架 | **`Tools/site_scons/`**（Python 包） | 工具链/平台/codegen/docgen/remote 适配 |
| 脚本 | **`Tools/scripts/`** | `setup_env`/`new_module`/`run_gtest`/`run_check`/`run_integration_test`/`remote_build` |

### B. 随仓库提交的离线工具 (`Tools/vendor/` → 解压到 `Tools/.toolcache/`，gitignore)
可再分发的工具/依赖已**提交进仓库**（Windows x86_64），`fetch_tools.py`/`setup_env.py`
**离线优先**从 `vendor/` 复制解压；缺该平台归档时才回退下载。版本+SHA-256 见
[`Tools/vendor/manifest.json`](vendor/manifest.json)。
| 工具 | 固定版本 | vendor 文件 | 用途 | 自动使用 |
|------|---------|------------|------|------|
| CMake (官方 portable) | `3.28.6` | `archives/cmake-3.28.6-windows-x86_64.zip` | 单元测试构建 | 系统无 cmake 时自动用 `.toolcache` 里的 |
| GoogleTest 源码 | `v1.14.0` | `archives/googletest-1.14.0.tar.gz` | 单元测试 | 自动作 `FETCHCONTENT_SOURCE_DIR`（不联网） |
| cppcheck (Win) | `2.13.0` | `installers/cppcheck-2.13.0-x64-Setup.msi` | `scons check` 静态/MISRA | `msiexec /a` 解压便携版，`run_check.py` 自动用 |
| scons / PyYAML | `4.10.1` / `6.0.1` | `wheels/*.whl`（cp39–cp312 win_amd64） | venv 依赖 | `setup_env` 离线 `pip --no-index` |

### C. 外部(系统)工具 —— 由系统/工具链提供（无法 vendoring；`setup_env.py --check` 核验+版本校验）
| 工具 | 必需性 | 用途 | 为何不放仓库 |
|------|--------|------|-----------|
| `gcc` / `g++` 或 MSVC/MinGW | host 验证/单测 | `TOOLCHAIN=gcc` 编译、GoogleTest | 编译器体积大, 用系统 |
| Python 解释器 ≥3.9 | 必需 | venv 基座 | 装原生 Windows Python（wheel 覆盖 cp39–cp312） |
| `git` | 必需 | 版本控制 | 系统基础工具 |
| `doxygen` / `rsync` / `ssh` | 可选 | `scons doc` / `--remote` | 系统按需 |

> Linux/CI 不用 `vendor/`（无 Windows 二进制）：cmake/cppcheck 用系统(`apt`)；
> `cmake`/`ctest`/`cppcheck` 始终系统优先，缺失才回退 `.toolcache`。

### D. 目标工具链 (TriCore / QEMU) —— 厂商授权 / 按需自建，不可再分发
| 工具 | 用途 | 获取方式（不放仓库） |
|------|------|------|
| `tricore-gcc` (HighTec GNU TriCore) | AURIX ELF（`TOOLCHAIN=hightec`） | HighTec 官网下载安装；装好后确保 `tricore-gcc` 在 PATH（或设 `HIGHTEC_BIN`） |
| TASKING VX-toolset | `TOOLCHAIN=tasking` | TASKING 授权安装；`ctc/ltc` 在 PATH（或设 `TASKING_BIN`） |
| `qemu-system-tricore` | 集成测试（第二道闸） | 用支持 TriCore 的 QEMU 分支自建；二进制在 PATH（或设 `QEMU_TRICORE`） |

> 这三者因**授权/体积/需自建**不放仓库；在**自托管 runner**（标签 `tricore`）或本机装好即可，`integration-qemu.yml` 与 `run_qemu.py`/`run_integration_test.py` 会自动调用，缺失则记 skipped。

## CI 一致性
- `.github/workflows/*` 用 `pip install -r Tools/requirements.txt`（同一 pinned 文件），`cache-dependency-path: Tools/requirements.txt`。
- 这样 CI 与本地（venv）用**同一份固定依赖**，结果可复现。

## 升级流程
- Python 依赖：改 `Tools/requirements.txt`，并把对应新 wheel 放进 `Tools/vendor/wheels/` → `python Tools/setup_env.py --force` 重建 venv。
- 离线工具（cmake/googletest/cppcheck）：换 `Tools/vendor/{archives,installers}/` 下的文件、改 `Tools/fetch_tools.py` 顶部版本常量、重算 `vendor/manifest.json` 的 sha256 → `python Tools/fetch_tools.py --force`。
- 改完跑 `scons compile` + `scons check`（含单测）验证 → 提交。

> 注：`Tools/.venv/` 与 `Tools/.toolcache/` 均已 gitignore（解压产物，不入库）；
> **入库的是 `Tools/vendor/`** 里的压缩归档/wheel（离线源），各机器据此复现同一版本。
