# OpenAutosar

OpenAutosar 是一个 **AUTOSAR Classic Platform** 嵌入式工程项目框架，面向
Infineon AURIX TC3xx (TC387 / TC367) 平台，覆盖从 MCAL、复杂驱动 (CDD)、
BSW、组件到上层应用 (ASW) 的完整分层结构，并集成构建 (SCons)、配置
(EB tresos / ETAS / Vector)、测试 (GoogleTest / QEMU) 与静态检查 (MISRA / QAC)。

> 完整的图文文档见 [`docs/index.html`](docs/index.html)（在浏览器中打开）。
> 信息安全分级与灵活编译方案见 [`docs/build_and_security.html`](docs/build_and_security.html)。
> Git 多子库划分方案见 [`docs/repo_structure.html`](docs/repo_structure.html)。

## Git 多子库（multi-repo）划分

仓库按文件夹拆分为独立子库，用 google-repo 清单组装：

- **源码**：每个顶层文件夹一个子库（`asw` / `bsw` / `cdd` / `mcal` / `components` / `projects` / `tools` / `test` / `docs`）。
- **共享库**：`mcal_libs`（按芯片），跨项目共享。
- **项目库**：`ASW_Libs/`、`CDD_Libs/`、`BSW_Libs/` 与项目相关，**按项目建子库** —— `asw_libs_<Project>`、`cdd_libs_<Project>`、`bsw_libs_<Project>`（扁平命名、下划线分隔），路径为 `<Layer>_Libs/<Project>/`。BSW 的 `.a` 已把该项目的 ECUC 配置生成代码一并打包，故也按项目。

子库清单见 `Tools/Git/repo_map.txt` 与 `manifests/default.xml`。创建子库（并在 GitHub 建仓+推送）：

```bat
REM 预览
Tools\Git\init_subrepos.bat https://github.com/<owner> --dry-run
REM gh CLI 建仓 + 推送 (先 gh auth login; 组织加 --org, 公开加 --public)
Tools\Git\init_subrepos.bat https://github.com/<owner> --github <owner> --push
REM 无 gh 时用 REST API: 先 set GITHUB_TOKEN=ghp_xxxx
```

详见 `Tools/Git/README.md` 与 `docs/repo_structure.html`。

## 信息安全分级（代码可见性隔离）

不同角色只能看到与其职责相关的源码，其余以二进制库（`.a` + 头文件）形式集成。根目录新增
`ASW_Libs/`、`BSW_Libs/`、`CDD_Libs/`、`MCAL_Libs/` 作为释放区；工程下新增
`Projects/<P>/Integration/` 作为基础工程师维护的集成代码。

| 角色 | 可见源码 | 二进制集成 (.a + inc) | manifest |
|------|----------|------------------------|----------|
| 平台/集成负责人 | 全部 | — | `manifests/default.xml` |
| ASW 工程师 | `ASWs/`、`Components/`、`Projects/` | `BSW_Libs`、`CDD_Libs`、`MCAL_Libs` | `manifests/manifest-asw.xml` |
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
| `ASWs/` | 上层应用软件：各应用模型代码（`<Model>/src` + `<Model>/inc`）或预编译 `.a` + `inc/` 头文件，`ASWs/inc/` 为公共对外头文件 |
| `BSW/` | 项目静态代码包（ETAS / Vector）：各供应商 `inc/` + 预编译 `lib/*.a`，`BSW/inc/` 为公共头文件 |
| `CDDs/` | 复杂驱动：每模块 `inc/`（头文件）、`src/`（静态代码）、`gen/`（配置代码），模块根目录有 `SConscript` 或 `.a` + `inc/` |
| `Components/` | 组件模块 / 公共代码；`Components/Mcu/Sbm/` 为启动代码模块（或 `.a` + `inc/`） |
| `MCAL/` | MCAL 静态代码：`Tc387/`、`Tc367/`，每模块 `inc/` + `src/` 或 `.a` + `inc/` |
| `Projects/` | 各项目工程，见下方“工程目录”说明 |
| `Tools/` | 项目工具（A2L、SCons、编译器封装等） |
| `Test/` | 测试：`ut/`（单元测试）、`Projects/`（集成测试/QEMU）、`QAC/`（静态分析） |
| `docs/` | HTML 文档 |
| `reports/` | 测试 / 检查报告输出 |
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

## 构建

```bat
REM 设置 TriCore 工具链
call Tools\Compiler\setup_env.bat

REM 构建某工程
cd Projects\Demo_Tc387
scons -j %NUMBER_OF_PROCESSORS%
```

## 测试

```bat
run_gtest.bat                 # 单元测试 + 覆盖率
run_integrationTest.bat Demo_Tc387   # QEMU 集成测试
```
