# OpenAutosar SCons 构建系统 — v2 评审后样板

本目录归档评审后的 v2 设计文档、脚本与配置文件。v2 在 `docs/prototype` 的基础上修正了首轮评审发现的可执行性问题，目标是让用户主要通过修改 `Projects/<Project>/build.yaml` 完成编译、代码生成、文档生成和远程构建配置。

关键改动：

- 正式支持 `scons --remote=<server>`、`scons --no-pull`、`scons --skip-gen`。
- `gen:<name>` 只触发指定代码生成器。
- 启用模块缺少 `SConscript` 时直接失败，避免配置错误被静默跳过。
- 增加 `schema_version: "2.0"` 和基础配置校验。
- 补齐 `doc-clean`、`doc:<Module>`、远程 rsync port/key 透传。
- `output.formats` 中的 `map` 现在真正控制 map 文件生成。

> **目标**：先在此目录内完成 review，确认无误后再按下方"部署步骤"复制到项目真实位置。

---

## 1. 目录结构

```
docs/v2/
├── README.md                          ← 本文件
├── requirements.txt                   ← Python 依赖清单
│
├── SConstruct                         ← 顶层入口（部署到项目根）
│
├── site_scons/                        ← 部署到项目根
│   └── autosar/
│       ├── __init__.py
│       ├── config_loader.py           ★ build.yaml 解析器
│       ├── module_builder.py          ★ 通用 BSW 模块构建器
│       ├── toolchain/                 工具链适配
│       │   ├── __init__.py
│       │   ├── arm_gcc.py             ✓ 完整实现
│       │   ├── arm_ghs.py             ⚠ 骨架（按需补全）
│       │   ├── arm_iar.py             ⚠ 骨架
│       │   ├── hightec.py             ⚠ 骨架
│       │   └── tasking.py             ⚠ 骨架
│       ├── platform/                  芯片平台适配
│       │   ├── __init__.py
│       │   ├── cortex_m33.py          ✓ 完整实现
│       │   ├── cortex_r5.py           ⚠ 骨架
│       │   ├── cortex_r52p.py         ⚠ 骨架
│       │   ├── aurix2g.py             ⚠ 骨架
│       │   ├── cypress.py             ✓ 完整实现
│       │   └── yuntu.py               ⚠ 骨架
│       ├── codegen/                   代码生成器适配
│       │   ├── __init__.py            工厂
│       │   ├── base.py                ★ 抽象基类
│       │   ├── vector.py              ✓ Vector DaVinci
│       │   ├── etas.py                ✓ ETAS ISOLAR
│       │   ├── eb_tresos.py           ✓ EB tresos
│       │   ├── turing.py              ✓ 图灵
│       │   └── custom.py              ✓ 自研/通用 shell hook
│       ├── docgen/                    文档生成
│       │   ├── __init__.py
│       │   ├── doxygen_runner.py
│       │   ├── interface_extractor.py
│       │   ├── dependency_grapher.py
│       │   └── index_builder.py
│       └── remote/                    远程 SSH 构建
│           ├── __init__.py
│           ├── ssh_runner.py
│           ├── sync.py
│           └── server_pool.py
│
├── scripts/                           辅助脚本（部署到项目根）
│   ├── remote_build.py
│   └── new_module.py                  脚手架：新建 BSW 模块
│
├── projects/                          项目级配置样板
│   └── DemoProject/                   部署到 Projects/DemoProject/
│       ├── build.yaml                 ★ 主配置（必含）
│       ├── build.lite.yaml            裁剪样例
│       ├── build.qemu.yaml            QEMU 样例
│       └── SConscript                 项目主脚本
│
├── doxygen/
│   └── Doxyfile.in                    Doxygen 模板（部署到 docs/）
│
└── demo_bsw/                          ★ 验证用样例模块（最小可编译）
    └── Crc/
        ├── inc/Crc.h
        ├── src/Crc.c
        └── SConscript
```

图例：✓ 完整实现，可直接用 | ★ 核心 | ⚠ 骨架/TODO

---

## 2. 安装依赖

```bash
pip install -r docs/v2/requirements.txt
```

可选系统依赖（按需）：

| 工具 | Windows | Linux |
|------|---------|-------|
| Doxygen | `choco install doxygen.install` | `apt install doxygen` |
| Graphviz | `choco install graphviz` | `apt install graphviz` |
| rsync | `choco install rsync` 或用 WSL | `apt install rsync` |
| ssh | Windows 10/11 内置 OpenSSH | 系统自带 |

---

## 3. 部署步骤（手动复制）

> 建议先在测试分支操作，确认无误后再合入主干。

### 3.1 Windows PowerShell

```powershell
$ROOT  = "D:\商林刚\03_Code\04_Autosar\01_OpenAutosar"
$PROTO = "$ROOT\docs\v2"

# ① 顶层入口
Copy-Item "$PROTO\SConstruct" "$ROOT\" -Force

# ② site_scons 扩展（核心 Python 包）
Copy-Item "$PROTO\site_scons" "$ROOT\" -Recurse -Force

# ③ scripts 辅助脚本
Copy-Item "$PROTO\scripts" "$ROOT\" -Recurse -Force

# ④ 项目级配置
Copy-Item "$PROTO\projects\DemoProject\*" `
          "$ROOT\Projects\DemoProject\" -Force

# ⑤ Doxygen 模板
New-Item -ItemType Directory -Path "$ROOT\docs" -Force | Out-Null
Copy-Item "$PROTO\doxygen\Doxyfile.in" "$ROOT\docs\" -Force

# ⑥ （可选）部署演示模块用于第一次验证
Copy-Item "$PROTO\demo_bsw\Crc" "$ROOT\BSW\" -Recurse -Force
```

### 3.2 Linux / Git Bash

```bash
ROOT="D:/商林刚/03_Code/04_Autosar/01_OpenAutosar"
PROTO="$ROOT/docs/v2"

cp    "$PROTO/SConstruct"             "$ROOT/"
cp -r "$PROTO/site_scons"             "$ROOT/"
cp -r "$PROTO/scripts"                "$ROOT/"
cp    "$PROTO/projects/DemoProject/"* "$ROOT/Projects/DemoProject/"
mkdir -p "$ROOT/docs"
cp    "$PROTO/doxygen/Doxyfile.in"    "$ROOT/docs/"
cp -r "$PROTO/demo_bsw/Crc"           "$ROOT/BSW/"
```

---

## 4. 验证流程（最小冒烟）

### Step 1：只编演示模块 Crc（确认链路通）

`Projects/DemoProject/build.yaml` 默认只启用 Crc 一个模块。直接运行：

```bash
cd D:\商林刚\03_Code\04_Autosar\01_OpenAutosar
scons -j8
```

期望输出：
```
[INFO] toolchain = arm_gcc
[INFO] platform  = CortexM33
[BUILD] BSW/Crc
arm-none-eabi-gcc -o Projects/DemoProject/Obj/.../Crc.o ...
arm-none-eabi-gcc -o Projects/DemoProject/Out/DemoProject_CYT4BF-A0.elf ...
scons: done building targets.
```

产物位置：`Projects/DemoProject/Out/DemoProject_CYT4BF-A0.elf`

### Step 2：验证动态模块开关

编辑 `Projects/DemoProject/build.yaml`，把 Crc 的 `enabled: true` 改成 `false`，重新运行：

```bash
scons
```

期望看到 `[SKIP] BSW/Crc` 且不再编译该模块。

### Step 3：验证代码生成（用 custom 适配器先试）

`build.yaml` 中的 `codegen.generators` 含一个 `type: custom` 示例（调用 echo 模拟生成）。运行：

```bash
scons gen
```

### Step 4：验证文档生成

```bash
scons doc
```

打开 `docs/api/index.html` 查看产物。

### Step 5：验证远程构建（如有可用服务器）

先在 `build.yaml` 中配置服务器，然后：

```bash
scons --remote=build_server_01
scons --remote=auto --no-pull
```

---

## 5. 渐进式接入真实 BSW 模块

确认 Crc 跑通后，按以下流程接入更多模块：

```bash
# 用脚手架自动生成 SConscript
python scripts/new_module.py --bsw CanIf
python scripts/new_module.py --bsw Com
python scripts/new_module.py --bsw Os
# ...
```

然后在 `Projects/DemoProject/build.yaml` 的 `bsw_modules:` 数组中追加：

```yaml
- name: CanIf
  enabled: true
- name: Com
  enabled: true
```

每加一个模块跑一次 `scons` 验证。

---

## 6. 回退方案

如果 SCons 构建出问题，原有 Makefile 仍然可用：

```bash
cd Projects/DemoProject
make all
```

SCons 不会修改任何 `.mak` 文件，二者可长期并行。

---

## 7. 已知 TODO

| 项 | 描述 | 优先级 |
|----|------|--------|
| 工具链 arm_ghs/iar/hightec/tasking | 需要补全实际编译命令与 flag | 高（按项目需要） |
| 平台 R5/R52P/AURIX2G/YunTu | 需要补全 CPU/FPU/链接器约束 | 中 |
| codegen 持久化进程模式 | 减少 Vector/EB 冷启动 30s+ | 中 |
| docgen Doxygen 调用 | 已留接口，需绑定 Doxyfile.in | 低 |
| remote.server_pool 自动调度 | 当前简化为指定 target | 低 |

参见设计文档 §13.1（迁移计划）的 P4-P7 阶段。
