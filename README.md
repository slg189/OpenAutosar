# OpenAutosar

OpenAutosar 是一个 **AUTOSAR Classic Platform** 嵌入式工程项目框架，面向
Infineon AURIX TC3xx (TC387 / TC367) 平台，覆盖从 MCAL、复杂驱动 (CDD)、
BSW、组件到上层应用 (ASW) 的完整分层结构，并集成构建 (SCons)、配置
(EB tresos / ETAS / Vector)、测试 (GoogleTest / QEMU) 与静态检查 (MISRA / QAC)。

> 完整的图文文档见 [`Projects/Demo_Tc387/Docs/index.html`](Projects/Demo_Tc387/Docs/index.html)（在浏览器中打开）。
> 信息安全分级与灵活编译方案见 [`Projects/Demo_Tc387/Docs/build_and_security.html`](Projects/Demo_Tc387/Docs/build_and_security.html)。
> Git 多子库划分方案见 [`Projects/Demo_Tc387/Docs/repo_structure.html`](Projects/Demo_Tc387/Docs/repo_structure.html)。

## Git 多子库（multi-repo）划分

仓库按文件夹拆分为独立子库，用 google-repo 清单组装：

- **共享源码**：`mcal`（供应商静态码，静态/配置分离、跨项目复用）及 `components` / `projects` / `tools`。`Test` / `Docs` / `Reports` 已随工程下沉到 `Projects/<P>/`，属 `projects` 子库。
- **BSW 供应商交付**：`BSW/` 不再分 `Vector` / `Etas` 目录，直接按模块名（`Os` / `Com` / …）组织；供应商、芯片、交付版本信息编码进 git 子库名 —— `BSW_<Vendor>_<Chip>_<Delivery>`（如 `BSW_Vector_TC387_CBDxxxxxx`、`BSW_Etas_TC387_RtaOs`），统一归属 **BSW group**，**每家供应商一个交付库**，各自检出到 `BSW/<模块>`，共同组成扁平的 `BSW/` 树。
- **项目源码**：`ASW/`、`CDD/` 模块代码**未按 AUTOSAR 静态/配置分离**，整体与项目相关，故源码也按项目 —— `asw_<Project>`、`cdd_<Project>`。
- **项目库**：`ASW_Libs/`、`CDD_Libs/`、`BSW_Libs/`、`MCAL_Libs/` **全部按项目建子库**。`asw_libs_<Project>`、`cdd_libs_<Project>`、`bsw_libs_<Project>`（扁平命名、下划线分隔；库目录按模块名、不分供应商）。**`MCAL_Libs/`** 针对项目 MCAL 芯片已固定（TC387/TC367 不会同时存在），故**只保留该项目的芯片、不分 `<Chip>` 一级**，芯片与项目身份编码进 git 子库名 —— 归 **MCAL group**、按项目区分：**`MCAL_<Project>`**。MCAL 的 `.a` 把配置代码与静态代码一并编译链接、BSW 的 `.a` 已打包项目 ECUC 配置，故均按项目。**工作区中这些库/源码直接检出到对应目录根**（不带项目/芯片子目录）——拉取时项目已由子库名确定。

子库清单见 `Tools/Git/repo_map.txt` 与 `manifests/default.xml`。创建子库（并在 GitHub 建仓+推送）：

```bat
REM 预览 (--project 决定 *_libs_<name>, 默认 Demo_Tc387)
Tools\Git\init_subrepos.bat https://github.com/<owner> --project Demo_Tc387 --dry-run
REM gh CLI 建仓 + 推送 (先 gh auth login; 组织加 --org, 公开加 --public)
Tools\Git\init_subrepos.bat https://github.com/<owner> --project Demo_Tc387 --github <owner> --push
REM 无 gh 时用 REST API: 先 set GITHUB_TOKEN=ghp_xxxx
```

详见 `Tools/Git/README.md` 与 `Projects/Demo_Tc387/Docs/repo_structure.html`。

## 信息安全分级（代码可见性隔离）

不同角色只能看到与其职责相关的源码，其余以二进制库（`.a` + 头文件）形式集成。根目录新增
`ASW_Libs/`、`BSW_Libs/`、`CDD_Libs/`、`MCAL_Libs/` 作为释放区；工程下新增
`Projects/<P>/Integration/` 作为基础工程师维护的集成代码。

| 角色 | 可见源码 | 二进制集成 (.a + inc) | manifest |
|------|----------|------------------------|----------|
| 平台/集成负责人 | 全部 | — | `manifests/default.xml` |
| ASW 工程师 | `ASW/`、`Components/`、`Projects/` | `BSW_Libs`、`CDD_Libs`、`MCAL_Libs` | `manifests/manifest-asw.xml` |
| 基础(集成)工程师 | `Projects/*/Integration/` | `ASW_Libs`、`BSW_Libs`、`CDD_Libs`、`MCAL_Libs` | `manifests/manifest-integration.xml` |

```bat
REM 按角色视图签出 (google-repo)
set MANIFEST_NAME=manifest-asw.xml && repo_metasar.bat init && repo_metasar.bat sync
REM 将源码模块编译并释放到 *_Libs/
release_libs.bat all
```

## 灵活编译（按模块灵活配置）

各模块在 `Projects/<P>/module_config.py` 中独立声明形态与编译选项；命令行可临时覆盖：

```bat
cd Projects/Demo_Tc387
scons                       # 按 module_config.py 构建
scons variant=Debug         # Debug / Release
scons chip=Tc367            # 切换目标芯片
scons only=Adc,CddPwm       # 仅编译指定模块
scons skip=EnergyManagement # 跳过指定模块
scons layers=ASW,MCAL       # 仅编译指定层
scons release               # 编译并把源码模块释放回 *_Libs
```

每个模块可配 `mode`（`source` 源码编译 / `lib` 库集成）、`enabled`、`cflags`（模块级编译选项）。

## 目录结构

| 目录 / 文件 | 说明 |
|-------------|------|
| `ASW/` | 上层应用软件：各应用模型代码（`<Model>/src` + `<Model>/inc`）或预编译 `.a` + `inc/` 头文件，`ASW/inc/` 为公共对外头文件 |
| `BSW/` | 项目 BSW 供应商**源码**：直接按模块名（`Os` / `Com` / `Crc` / …）组织，每模块 `inc/` + `src/` + `SConscript`，`BSW/inc/` 为公共头文件。编译产物 `.a` 释放到 `BSW_Libs/`（非源码可见者只拿二进制）。供应商/芯片/交付版本不在目录中，而在 git 子库名（`BSW_<Vendor>_<Chip>_<Delivery>`，BSW group）中 |
| `CDD/` | 复杂驱动：每模块 `inc/`（头文件）、`src/`（静态代码）、`gen/`（配置代码），模块根目录有 `SConscript` 或 `.a` + `inc/` |
| `Components/` | 组件模块 / 公共代码；`Components/Mcu/Sbm/` 为启动代码模块（或 `.a` + `inc/`） |
| `MCAL/` | MCAL 静态代码：`Tc387/`、`Tc367/`，每模块 `inc/` + `src/` 或 `.a` + `inc/` |
| `Projects/` | 各项目工程，见下方“工程目录”说明 |
| `Tools/` | 项目工具（A2L、SCons、编译器封装、Doxygen 模板等） |
| `manifests/` | google-repo 工作区组装清单（须位于顶层，bootstrap 整个工作区） |
| `SConstruct` | **v2 构建框架顶层入口**（YAML 驱动，见“构建”） |
| `site_scons/autosar/` | v2 构建框架（工具链/平台/代码生成/文档/远程 适配） |
| `scripts/` | 框架脚本：`new_module.py`（模块脚手架）、`remote_build.py` |
| `requirements.txt` | 构建框架 Python 依赖（pyyaml 等） |
| `.clang-format` | 代码格式规范 |
| `.gitignore` | 忽略构建产物与生成代码 |
| `misra.json` | MISRA C:2012 静态检查配置 |
| `repo_metasar.bat` | google-repo manifest 多仓同步脚本 |
| `run_gtest.bat` | 构建并运行 GoogleTest 单元测试 + 覆盖率 |
| `run_integrationTest.bat` | 在 QEMU 上运行集成测试 |
| `TestPlatform_UintTest_QEMU_Test.json` | QEMU 测试平台配置 |

### 工程目录 `Projects/<Project>/`

| 子目录 | 说明 |
|--------|------|
| `Cfg/{MCAL,CDD,BSW}/` | MCAL 的 EB tresos 工程 与 BSW 的 ETAS 工程配置文件（ECUC `*.arxml` 等） |
| `Ld/` | 链接脚本 |
| `Gen/{MCAL,CDD,BSW}/` | 由配置工具生成的配置代码（构建产物） |
| `Obj/` | 编译临时 `.o` 文件 |
| `Out/` | 链接输出 `.hex` / `.elf` / `.map` |
| `Libs/{ASW,BSW,CDD,MCAL}/` | 各层编译生成的静态库 `.a` |
| `build.yaml` | **v2 框架项目主配置**（用户主要改这份；模块开关/代码生成/文档/远程） |
| `SConscript` | v2 框架项目脚本（按 `build.yaml` 动态组织构建） |
| `SConstruct` | 旧 per-project 构建入口（回退方案，与 v2 并存） |
| `Test/` | 工程测试：`ut/`（单元测试）、`Projects/`（集成测试/QEMU）、`QAC/`（静态分析） |
| `Docs/` | 工程文档（HTML；`Docs/build-system/` 为 v2 构建框架设计存档与参考样例） |
| `Reports/` | 测试 / 检查报告输出 |

## 构建

### v2 构建框架（YAML 驱动，推荐）

用户主要修改 `Projects/<Project>/build.yaml` 即可完成 编译 / 代码生成 / 文档 / 远程构建。
从**仓库根**运行：

```bat
pip install -r requirements.txt          REM 一次性: 安装 pyyaml 等
call Tools\Compiler\setup_env.bat         REM 设置 TriCore 工具链 (hightec/tasking)

scons -j %NUMBER_OF_PROCESSORS%           REM 构建默认项目 Demo_Tc387 -> Out\Demo_Tc387.elf
scons TOOLCHAIN=tasking PLATFORM=AURIX2G  REM 命令行覆盖工具链/平台
scons gen                                 REM 只跑代码生成
scons doc                                 REM 生成接口/依赖/索引文档
scons --remote=build_server_01            REM 远程服务器构建
scons PROJECT=Projects/Demo_Tc387 CONFIG=build.yaml
```

> 框架分层：根 `SConstruct` → `site_scons/autosar/**`（框架）→ `Projects/<P>/SConscript` → 各模块 `SConscript`。
> 设计与迁移计划见 [`Projects/Demo_Tc387/Docs/build-system/`](Projects/Demo_Tc387/Docs/build-system/)。
> 新增模块脚手架：`python scripts/new_module.py --bsw <Name>`。

> **当前进度（Phase 1）**：框架已落地仓库根，AURIX（`hightec`/`AURIX2G`）目标已打通，
> 冒烟模块 `BSW/Crc` 可经 `build.yaml` 构建。现有 MCAL/CDD/ASW 模块迁为 v2 原生 `SConscript`
> 属 Phase 2（见 build-system 文档）。

### 旧 per-project 构建（回退方案，仍可用）

```bat
cd Projects\Demo_Tc387
scons -j %NUMBER_OF_PROCESSORS%           REM 走 Projects\Demo_Tc387\SConstruct + Tools\Scons\build_helpers.py
```

## 测试

```bat
run_gtest.bat                 # 单元测试 + 覆盖率
run_integrationTest.bat Demo_Tc387   # QEMU 集成测试
```
