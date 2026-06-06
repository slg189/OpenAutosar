# OpenAutosar SCons Build System Design v2

## 1. 归档目的

本目录是对 `docs/prototype` 和 `docs/SCons_Build_System_Design.html` 首轮评审后的 v2 归档。v2 的目标不是立即替换仓库根目录构建系统，而是把评审后的脚本、配置和迁移规则集中放到 `docs/v2`，便于二次评审、冒烟验证和后续部署。

核心要求：用户修改 `Projects/<Project>/build.yaml` 后，应能直接完成本地编译、代码生成、文档生成和远程编译；用户不应再修改加密 Makefile，也不应为了普通项目裁剪去改 SCons 框架代码。

## 2. 评审问题与 v2 处理

| 评审问题 | v2 处理方式 |
| --- | --- |
| `scons --remote=xxx` 没有注册，命令可能直接失败 | 在 `SConstruct` 中通过 `AddOption()` 注册 `--remote`、`--no-pull`、`--skip-gen` |
| `gen:<name>` 实际绑定全部生成器 | `Projects/DemoProject/SConscript` 使用 `codegen_by_name`，每个 alias 只指向自己的 target |
| 启用模块缺少 `SConscript` 时只是 warn 并跳过 | 改为 fail-fast，提示关闭模块或补齐模块脚本 |
| `build.yaml` 缺少 schema 和友好错误 | 增加 `schema_version: "2.0"`，`config_loader.py` 做基础节点、必填字段、未知顶层字段校验 |
| `doc-clean`、`doc:<Module>` 设计有但脚本没有 | `SConscript` 注册 `doc-clean` 和每个启用模块的 `doc:<Module>` |
| `output.formats: map` 未真正控制 map | 从 `arm_gcc.py` 移除默认 map flag，由项目脚本按 `output.formats` 注入 |
| 远程同步未透传 port/key | `remote/sync.py` 为 rsync 增加 `-e ssh ...` transport |
| 代码生成 stamp 依赖系统 `python` 命令 | `codegen/base.py` 改为 Python Action 函数执行命令并写 stamp |

## 3. v2 文件布局

```
docs/v2/
├── SConstruct
├── projects/DemoProject/
│   ├── build.yaml
│   ├── build.lite.yaml
│   ├── build.qemu.yaml
│   └── SConscript
├── site_scons/autosar/
│   ├── config_loader.py
│   ├── module_builder.py
│   ├── codegen/
│   ├── docgen/
│   ├── remote/
│   ├── platform/
│   └── toolchain/
├── scripts/
├── doxygen/Doxyfile.in
├── demo_bsw/Crc/
├── deploy.ps1
├── deploy.sh
└── SCons_Build_System_Design_v2_Reviewed.md
```

## 4. 用户配置边界

项目用户应主要修改以下文件：

- `Projects/<Project>/build.yaml`
- 可选的 `Projects/<Project>/build.lite.yaml`
- 可选的 `Projects/<Project>/build.qemu.yaml`

框架维护者负责维护以下内容：

- 顶层 `SConstruct`
- `site_scons/autosar/**`
- 每个模块自己的 `SConscript`
- `scripts/new_module.py`
- `docs/Doxyfile.in`

如果用户在 `build.yaml` 中启用某个模块，而该模块没有 `SConscript`，v2 会直接失败。这是有意设计，避免出现“配置写了但模块没有编译”的隐性风险。

## 5. v2 命令接口

| 命令 | 行为 |
| --- | --- |
| `scons -j8` | 本地构建默认项目 |
| `scons PROJECT=DemoProject CONFIG=build.lite.yaml` | 构建指定项目配置 |
| `scons TOOLCHAIN=arm_gcc PLATFORM=CortexM33` | 命令行覆盖目标工具链和平台 |
| `scons gen` | 运行所有启用的代码生成器 |
| `scons gen:demo_echo` | 只运行指定代码生成器 |
| `scons --skip-gen` | 编译时跳过代码生成 |
| `scons doc` | 生成项目文档索引、接口清单和依赖图 |
| `scons doc:Crc` | 生成指定模块接口文档 |
| `scons doc-clean` | 清理生成的 API 文档 |
| `scons --remote=build_server_01` | 指定远程服务器构建 |
| `scons --remote=auto --no-pull` | 自动选服务器构建且不拉回产物 |

## 6. 迁移要求

1. 每个真实模块必须有自己的 `SConscript`。
2. 新增模块先用 `scripts/new_module.py` 生成脚手架，再按真实源码目录补充特殊源文件、头文件和 flags。
3. `build.yaml` 只声明“本项目启用什么”，不承载复杂编译规则。
4. 公共编译逻辑放到 `site_scons/autosar/module_builder.py`。
5. 工具链差异放到 `site_scons/autosar/toolchain/<name>.py`。
6. 芯片平台差异放到 `site_scons/autosar/platform/<name>.py`。

## 7. 仍待落地事项

- 为 70+ BSW 模块批量生成并校正 `SConscript`。
- 将现有 `*_defs.mak` / `*_rules.mak` 中的真实源文件、特殊 flags、链接依赖迁移到模块脚本或 manifest。
- 完成 arm_ghs、arm_iar、hightec、tasking 的实际命令和 flags。
- 将 `compile_commands.json` 在目标 SCons 版本上做一次实机验证。
- 为 `build.yaml` 增加完整 JSON Schema 或 Pydantic 校验。
- 在 CI 中增加 `scons gen`、`scons doc`、最小 Crc 冒烟构建。

## 8. 推荐验收门槛

v2 部署到测试分支后，至少通过以下检查：

1. `scons -j8` 能生成 `Projects/DemoProject/Out/DemoProject_CYT4BF-A0.elf`。
2. 将 `Crc.enabled` 改为 `false` 后，构建不再编译 Crc。
3. `scons gen:demo_echo` 只触发 `demo_echo`。
4. `scons doc` 生成 `docs/api/index.html`。
5. `scons doc:Crc` 生成 `docs/api/modules/Crc/interfaces.md`。
6. 配置不存在的模块时构建失败并给出明确错误。
7. 远程构建能使用配置中的 port/key，并按 `pull_back` 回传产物。
