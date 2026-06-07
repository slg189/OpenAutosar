# OpenAutosar 构建系统 — 文档与设计存档

> ✅ **当前构建 = v3**（决策 D1 切到 v3 / D2 自动发现+显式清单 / D3 schema 3.0 / D4 含 release+check）。
> 旧构建（per-project `SConstruct` + `Tools/Scons/build_helpers.py` + `module_config.py`）与每模块
> `SConscript` 已退役（git 历史可查）。**从仓库根直接 `scons` 即可。**

## 落地位置

| 内容 | 位置 |
|------|------|
| 顶层入口 | 仓库根 `SConstruct` |
| 框架包 | `Tools/site_scons/{workspace,config_loader,resolver,module_builder,report,toolchain,platform,codegen,docgen,remote}` |
| 脚本 | `Tools/scripts/`（`new_module.py`、`remote_build.py`） |
| 依赖 | `Tools/requirements.txt`（固定版本）+ `Tools/setup_env.py`（隔离 venv）；环境说明见 `Tools/ENVIRONMENT.md` |
| Doxygen 模板 | `Tools/Doxygen/Doxyfile.in` |
| 项目主配置 | `Projects/<P>/build.yaml`（schema 3.0，`layers/mode`） |
| 项目编排器 | `Projects/<P>/SConscript`（按 `build.yaml` 自动发现+决策 source/lib 并构建） |

## 常用命令（仓库根运行）

```bash
scons                       # 构建默认项目 -> Out/<App>.elf
scons list                  # 打印模块/mode/来源 (source 或 *_Libs)
scons explain:<Mod>       # 解释某模块为何 source/lib
scons only=Adc,CddPwm       # 仅构建指定模块
scons skip=EnergyManagement # 跳过指定模块
scons layers=MCAL,ASW       # 仅构建指定层
scons gen | doc | release | check
scons TOOLCHAIN=tasking PLATFORM=AURIX2G   # 命令行覆盖目标
scons --remote=build_server_01             # 远程构建
```

## 设计文档

| 文档 | 说明 |
|------|------|
| [`ARCHITECTURE_PROPOSAL.md`](ARCHITECTURE_PROPOSAL.md) · [`ARCHITECTURE_PROPOSAL.html`](ARCHITECTURE_PROPOSAL.html) | v3 架构提案（含原框架 vs v2 对比、目标 `build.yaml`、解析算法、迁移路线） |
| [`DEVELOPMENT_PLAN.md`](DEVELOPMENT_PLAN.md) | 开发计划（决策门 D1–D4、里程碑 M0–M5、进度勾选表） |
| [`ENV_VALIDATION.html`](ENV_VALIDATION.html) | **构建环境验证**：用 host gcc 真实编译全部源码模块（不依赖 TriCore），结果 + 抓到的 bug |
| [`ENV_VALIDATION_PLAN.md`](ENV_VALIDATION_PLAN.md) | 环境验证开发计划（host gcc 门禁 → CI → 目标工具链/QEMU 第二道闸） |
| [`SCons_Build_System_Design_v2_Reviewed.md`](SCons_Build_System_Design_v2_Reviewed.md) | 历史存档：v2（首轮评审后）设计，已由 v3 取代 |

> 注：原 v2 的 Cortex-M 参考样例（`projects/DemoProject`）与手动部署脚本（`deploy.ps1`/`deploy.sh`）
> 已随 v3 落地删除——框架已就位，无需再手动部署。
