# OpenAutosar

[![Build Validation](https://github.com/slg189/OpenAutosar/actions/workflows/build-validation.yml/badge.svg)](https://github.com/slg189/OpenAutosar/actions/workflows/build-validation.yml)

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
`scons release`
```

## 灵活编译（按模块灵活配置）

各模块在 `Projects/<P>/build.yaml` 的 `layers` 下独立声明 `mode`(source|lib)、`enabled`、
`cflags`/`defines`/`includes`；命令行可临时覆盖工具链/平台/构建类型：

```bat
scons                            # 按 build.yaml 构建 (仓库根运行)
scons BUILD_TYPE=Debug           # Debug / Release
scons TOOLCHAIN=tasking          # 切换工具链 (hightec | tasking)
scons PLATFORM=AURIX2G           # 切换平台
scons only=Adc,CddPwm            # 仅编译指定模块 (名或 leaf)
scons skip=EnergyManagement      # 跳过指定模块
scons layers=MCAL,ASW            # 仅编译指定层
scons list                       # 打印模块/mode/来源 (source 或 *_Libs)
scons release                    # 编译并把源码模块释放回 *_Libs
```

> 模块级声明在 `build.yaml`（`enabled` / `mode` / `cflags` …）；`only/skip/layers` 为命令行临时过滤。

每个模块可配 `mode`（`source` 源码编译 / `lib` 库集成）、`enabled`、`cflags`（模块级编译选项）。

## 目录结构

| 目录 / 文件 | 说明 |
|-------------|------|
| `ASW/` | 上层应用软件：各应用模型代码（`<Model>/src` + `<Model>/inc`）或预编译 `.a` + `inc/` 头文件，`ASW/inc/` 为公共对外头文件 |
| `BSW/` | 项目 BSW 供应商**源码**：直接按模块名（`Os` / `Com` / `Crc` / …）组织，每模块 `inc/` + `src/`（按约定自动发现，无需 SConscript），`BSW/inc/` 为公共头文件。编译产物 `.a` 释放到 `BSW_Libs/`（非源码可见者只拿二进制）。供应商/芯片/交付版本不在目录中，而在 git 子库名（`BSW_<Vendor>_<Chip>_<Delivery>`，BSW group）中 |
| `CDD/` | 复杂驱动：每模块 `inc/`（头文件）、`src/`（静态代码）、`gen/`（配置代码）；或预编译 `.a` + `inc/`（lib 模式） |
| `Components/` | 组件模块 / 公共代码；`Components/Mcu/Sbm/` 为启动代码模块（或 `.a` + `inc/`） |
| `MCAL/` | MCAL 静态代码：`Tc387/`、`Tc367/`，每模块 `inc/` + `src/` 或 `.a` + `inc/` |
| `Projects/` | 各项目工程，见下方“工程目录”说明 |
| `Tools/` | 项目工具：`site_scons/`（v3 构建框架包）、`scripts/`、A2L、Compiler、Doxygen、Git |
| `Tools/site_scons/` | **v3 构建框架**（Python 包：工具链/平台/代码生成/文档/远程 适配 + workspace/resolver/...）；由 `Tools` 加入 sys.path、按包名 `site_scons` 导入 |
| `Tools/scripts/` | 跨平台 Python 脚本：`new_module.py`（模块骨架）、`run_gtest.py`（单元测试+覆盖率）、`run_integration_test.py`（QEMU 集成测试）、`remote_build.py`（远程构建） |
| `manifests/` | google-repo 工作区组装清单（须位于顶层，bootstrap 整个工作区） |
| `SConstruct` | **v3 构建框架顶层入口**（YAML 驱动，见“构建”） |
| `requirements.txt` | 构建框架 Python 依赖（pyyaml 等） |
| `.clang-format` | 代码格式规范 |
| `.gitignore` | 忽略构建产物与生成代码 |
| `misra.json` | MISRA C:2012 静态检查配置 |
| `repo_metasar.bat` | google-repo manifest 多仓同步脚本（Linux 下可直接用 `repo`） |
| `TestPlatform_UintTest_QEMU_Test.json` | QEMU 测试平台配置 |
| 释放 / 检查 | 不再用 `.bat`：源码模块释放用 `scons release`，静态检查+单测入口 `scons check` |

### 工程目录 `Projects/<Project>/`

| 子目录 | 说明 |
|--------|------|
| `Cfg/{MCAL,CDD,BSW}/` | MCAL 的 EB tresos 工程 与 BSW 的 ETAS 工程配置文件（ECUC `*.arxml` 等） |
| `Ld/` | 链接脚本 |
| `Gen/{MCAL,CDD,BSW}/` | 由配置工具生成的配置代码（构建产物） |
| `Obj/` | 编译临时 `.o` 文件 |
| `Out/` | 链接输出 `.hex` / `.elf` / `.map` |
| `Libs/{ASW,BSW,CDD,MCAL}/` | 各层编译生成的静态库 `.a` |
| `build.yaml` | **v3 框架项目主配置**（用户主要改这份；`layers/mode` 模块开关、source/lib、代码生成/文档/远程） |
| `SConscript` | v3 项目编排器（按 `build.yaml` 自动发现+决策 source/lib 并构建；无需每模块脚本） |
| `Test/` | 工程测试：`ut/`（单元测试）、`Projects/`（集成测试/QEMU）、`QAC/`（静态分析） |
| `Docs/` | 工程文档（HTML；`Docs/build-system/` 为构建框架设计/提案/开发计划） |
| `Reports/` | 测试 / 检查报告输出 |

## 构建（v3 框架，YAML 驱动）

用户主要修改 `Projects/<Project>/build.yaml`（`layers/mode` 模型）即可完成 编译 / 代码生成 / 文档 / 静态检查 / 远程构建。
从**仓库根**运行：

```bat
pip install -r requirements.txt          REM 一次性: 安装 pyyaml 等
call Tools\Compiler\setup_env.bat         REM 设置 TriCore 工具链 (hightec/tasking)

scons -j %NUMBER_OF_PROCESSORS%           REM 构建默认项目 Demo_Tc387 -> Out\Demo_Tc387.elf
scons list                                REM 打印解析后的模块/mode/来源 (source 或 *_Libs)
scons --explain Adc                        REM 解释某模块为何 source/lib
scons TOOLCHAIN=tasking PLATFORM=AURIX2G  REM 命令行覆盖工具链/平台
scons gen                                 REM 只跑代码生成
scons doc                                 REM 生成接口/依赖/索引文档
scons release                             REM 源码模块编译并释放 .a+inc 到 *_Libs
scons check                               REM 静态检查(MISRA)+单元测试入口
scons --remote=build_server_01            REM 远程服务器构建
```

> **模型**：`build.yaml` 以 `layers/mode` 统一表达五层；`defaults.visibility: auto` 时框架按
> “工作区是否可见源码”（manifest 决定）自动选 编译 / 链接 `*_Libs`，亦可显式 `mode: source|lib`。
> 模块**自动发现**（目录即模块），`build.yaml` 显式列模块、只写例外；`scons list` 提示未列入的模块。
>
> 框架分层：根 `SConstruct` → `Tools/site_scons/**`（框架）→ `Projects/<P>/SConscript`（编排器）。
> 设计/提案/开发计划见 [`Projects/Demo_Tc387/Docs/build-system/`](Projects/Demo_Tc387/Docs/build-system/)。
> 新增模块：把目录放进对应层（含 `inc/`+`src/`）即被发现；或 `python Tools/scripts/new_module.py`。
>
> 注：原 `Tools/Scons/build_helpers.py` + per-project `SConstruct` + `module_config.py` 的旧构建已退役（git 历史可查）。

## 测试（跨平台 Python，取代旧 `.bat`）

```bash
python Tools/scripts/run_gtest.py                 # 单元测试 + 覆盖率
python Tools/scripts/run_integration_test.py      # QEMU 集成测试 (需 TriCore 工具链构建 ELF)
scons check                                       # 静态检查(MISRA/cppcheck) + 单测入口
```
