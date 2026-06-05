# OpenAutosar

OpenAutosar 是一个 **AUTOSAR Classic Platform** 嵌入式工程项目框架，面向
Infineon AURIX TC3xx (TC387 / TC367) 平台，覆盖从 MCAL、复杂驱动 (CDD)、
BSW、组件到上层应用 (ASW) 的完整分层结构，并集成构建 (SCons)、配置
(EB tresos / ETAS / Vector)、测试 (GoogleTest / QEMU) 与静态检查 (MISRA / QAC)。

> 完整的图文文档见 [`docs/index.html`](docs/index.html)（在浏览器中打开）。

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
| `repo_metasar.sh` | google-repo manifest 多仓同步脚本 |
| `run_gtest.sh` | 构建并运行 GoogleTest 单元测试 + 覆盖率 |
| `run_integrationTest.sh` | 在 QEMU 上运行集成测试 |
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

```bash
# 设置 TriCore 工具链
source Tools/Compiler/setup_env.sh

# 构建某工程
cd Projects/Demo_Tc387 && scons -j$(nproc)
```

## 测试

```bash
./run_gtest.sh                 # 单元测试 + 覆盖率
./run_integrationTest.sh Demo_Tc387   # QEMU 集成测试
```
