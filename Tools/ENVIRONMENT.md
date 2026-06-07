# 构建环境 (跨机器一致)

目标: 把构建所需工具尽量**收进 `Tools/`、固定版本**，减少“不同电脑环境不一致”导致的问题。

## 一键搭建

```bash
python Tools/setup_env.py          # ① 检查/版本校验 ② 下载固定工具(cmake/googletest)到 .toolcache ③ 建隔离 venv + pinned Python 依赖
python Tools/setup_env.py --check  # 只检查外部工具 + 版本硬校验 (cmake>=3.16)
python Tools/setup_env.py --no-fetch  # 跳过下载工具 (只建 venv)
python Tools/fetch_tools.py        # 单独: 把可分发工具下载固定到 Tools/.toolcache (gitignore)
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

### B. 下载固定到仓库的工具 (`Tools/fetch_tools.py` → `Tools/.toolcache/`，gitignore)
开源/可再分发的工具按固定版本下载到 `.toolcache`，不提交二进制（不撑大仓库），各机器版本一致、可离线复用。
| 工具 | 固定版本 | 用途 | run_gtest 自动使用 |
|------|---------|------|------|
| CMake (官方 portable) | `3.28.6` | 单元测试构建 | 系统无 cmake 时自动用 `.toolcache` 里的 |
| GoogleTest 源码 | `v1.14.0` | 单元测试 | 自动作 `GOOGLETEST_DIR`（FetchContent 不联网） |

### C. 外部(系统)工具 —— 由系统/工具链提供（不便下载；`setup_env.py --check` 核验+版本校验）
| 工具 | 必需性 | 用途 | 为何不下载 |
|------|--------|------|-----------|
| `gcc` / `g++` | host 验证/单测 | `TOOLCHAIN=gcc` 编译、GoogleTest | 体积大, 用系统 |
| `cppcheck` | 静态检查 | `scons check`（cppcheck/MISRA） | Linux 无官方 portable, 用 `apt`/源码构建 |
| `git` | 必需 | 版本控制 | 系统基础工具 |
| `doxygen` / `rsync` / `ssh` | 可选 | `scons doc` / `--remote` | 系统按需 |

> `cmake`/`ctest` 优先用系统; 若系统没有, `fetch_tools` 下载的版本兜底。

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
- Python 依赖：改 `Tools/requirements.txt` → `python Tools/setup_env.py --force` 重建 venv。
- 下载工具（cmake/googletest）：改 `Tools/fetch_tools.py` 顶部 `CMAKE_VERSION`/`GTEST_TAG` → `python Tools/fetch_tools.py --force`。
- 改完跑 `scons compile` + `scons check`（含单测）验证 → 提交。

> 注：`Tools/.venv/` 与 `Tools/.toolcache/` 均已 gitignore（不提交二进制，靠脚本各机器复现同一版本）。
