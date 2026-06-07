# 构建环境 (跨机器一致)

目标: 把构建所需工具尽量**收进 `Tools/`、固定版本**，减少“不同电脑环境不一致”导致的问题。

## 一键搭建

```bash
python Tools/setup_env.py          # 建仓库内隔离 venv (Tools/.venv) + 装 pinned Python 依赖 + 检查外部工具
python Tools/setup_env.py --check  # 只检查外部工具是否齐备
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

### B. 外部(系统)工具 —— 由系统/工具链提供（`setup_env.py --check` 会逐项核验）
| 工具 | 必需性 | 用途 | 备注 |
|------|--------|------|------|
| `gcc` / `g++` | host 验证/单测 | `TOOLCHAIN=gcc` 编译、GoogleTest | 任一较新版本 |
| `cmake` ≥ 3.16 / `ctest` | 单元测试 | `run_gtest.py` | 实测 3.28 |
| `cppcheck` | 静态检查 | `scons check`（cppcheck/MISRA） | 实测 2.13 |
| `git` | 必需 | 版本控制 + GoogleTest FetchContent | |
| `doxygen` | 可选 | `scons doc` | 不装则跳过 |
| `rsync` / `ssh` | 可选 | `scons --remote` 远程构建 | |
| `tricore-gcc`(hightec) | 目标构建 | AURIX ELF（`TOOLCHAIN=hightec`） | 厂商工具链, 由 runner/本机提供 |
| `qemu-system-tricore` | 集成测试 | 第二道闸（`integration-qemu.yml`） | 自托管 runner |

> 外部工具不便“放进仓库”（体积/授权），但 `setup_env.py --check` 会核验并提示缺失，CI 里按需 `apt install`（cppcheck）或自托管 runner 预装（TriCore/qemu）。

### C. 第三方源码 —— 固定版本 + 可离线
| 项 | 固定方式 | 离线 |
|----|---------|------|
| GoogleTest | `Test/ut/CMakeLists.txt` 固定 `GIT_TAG v1.14.0`，FetchContent 拉取 | 设环境变量 `GOOGLETEST_DIR=<本地 googletest 源码>` → `run_gtest.py` 传 `-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST`，不联网 |

## CI 一致性
- `.github/workflows/*` 用 `pip install -r Tools/requirements.txt`（同一 pinned 文件），`cache-dependency-path: Tools/requirements.txt`。
- 这样 CI 与本地（venv）用**同一份固定依赖**，结果可复现。

## 升级流程
改 `Tools/requirements.txt`（或 GoogleTest tag）→ 本地 `python Tools/setup_env.py --force` 重建 → 跑 `scons compile/check` 验证 → 提交。
