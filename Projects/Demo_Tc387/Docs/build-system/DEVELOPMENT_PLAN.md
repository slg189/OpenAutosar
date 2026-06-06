# 构建系统 v3 开发计划

| 项 | 内容 |
|----|------|
| 配套架构 | [`ARCHITECTURE_PROPOSAL.md`](ARCHITECTURE_PROPOSAL.md) / [`ARCHITECTURE_PROPOSAL.html`](ARCHITECTURE_PROPOSAL.html) |
| 状态 | 待评审通过后启动（M0 是评审本身） |
| 总目标 | 用 v2 架构承载原框架领域模型 + 约定优于配置，**用户只改一份 `build.yaml`、且只写例外** |
| 硬约束 | ① 全程与旧构建**双套并存、零回归**；② 每里程碑可独立提交/独立回退；③ 受 D1–D4 决策门控 |
| 工时口径 | 下文「估」为粗略人日（perfect-day），仅供排序，非承诺 |

> 本计划是 [架构提案 §9 迁移路线] 的任务级展开。每个里程碑 = 一个可评审 PR/提交段。

---

## 0. 工作流总则

- **分支**：继续用当前开发分支；每个里程碑一组提交，信息以 `build-v3(Mx): ...` 前缀。
- **验证**：无目标工具链时统一用 `scons -n`（dry-run）+ stub `tricore-gcc` 校验构建图；有工具链的环境再做真编。
- **不破坏**：任何里程碑不得改坏 `cd Projects/Demo_Tc387 && scons`（旧构建）。
- **可发现**：凡引入“自动推断”，同批次必须提供 `scons list` 能打印该推断。

---

## 1. 决策门（M0 必须先定）

| # | 问题 | 默认（推荐） | 影响 |
|---|------|------------|------|
| D1 | 旧构建取舍 | **双套并存**，M5 再议退役 | 决定模块脚本是否需双兼容 |
| D2 | 自动发现 vs 显式清单 | **默认全发现 + 写例外** | 决定 `build.yaml` 体量与 M1 范围 |
| D3 | schema 版本 | `build.yaml` 升 **`3.0`**（`layers/mode`），与 `2.0` 共存 | 决定 `config_loader` 兼容策略 |
| D4 | 本轮范围 | 先做**编译链路**；`release/check` 放 M5 | 决定 M5 工作量 |

> 未拍板前不动代码。下文按默认值编排；若 D1=“切到 v3”，删去“双兼容/回退”相关任务。

---

## 2. 里程碑总览

| 里程碑 | 主题 | 估(人日) | 依赖 | 退出条件 |
|--------|------|:---:|------|---------|
| **M0** | 评审 + 决策 | 0.5 | — | D1–D4 拍板 |
| **M1** | 模块自动发现 + 通用 builder | 3 | M0 | `scons list` 列 BSW；BSW 三模块免 SConscript；不回归 |
| **M2** | 统一 `layers/mode` schema + `resolver` | 4 | M1 | 能按 `mode`/可见性 在 编译↔链接 间切换 |
| **M3** | 灰度纳入真实层 MCAL→CDD→ASW→Components | 5 | M2 | 各层 `scons -n` 构建图通过；旧构建仍可用 |
| **M4** | 对齐信息安全三视图 | 2 | M3 | 切 `manifest-asw/integration` 后自动走 `*_Libs` |
| **M5** | 收尾 `release/check/doc` + 文档/清理 | 3 | M4 | §架构提案 §13 验收全过 |
| | **合计** | **~17.5** | | |

---

## 3. 里程碑明细

### M1 · 模块自动发现 + 通用 builder
**目的**：消灭“每模块一份 SConscript”，让 `build.yaml` 不必枚举模块。

任务
- [ ] `site_scons/autosar/workspace.py`：`discover(layer_roots)` 按约定（含 `inc/` 目录）扫描模块；MCAL 支持 `<chip>/<mod>`。
- [ ] `module_builder.py`：增强为“无 SConscript 也能按约定编译”（`src/*.c|*.S` + `inc/` + `Gen/<mod>/`）。
- [ ] 项目 `SConscript`：改为“遍历 discover 结果”，删除“缺 SConscript 即 fail”的硬约束（保留显式 SConscript 优先级）。
- [ ] `report.py` + `scons list`：打印发现到的模块。
- [ ] 回归：`BSW/{Crc,Os,Com}` 删除各自 `SConscript` 后仍能 `scons -n` 编译链接。

交付物：`workspace.py`、增强版 `module_builder.py`、`report.py`、更新的项目 `SConscript`。
退出：`scons list` 列出 BSW 三模块；`scons -n` 产物不变。

### M2 · 统一 `layers/mode` schema + `resolver`
**目的**：一个 `mode: source|lib` 表达五层 + 预编译库 + 信息安全。

任务
- [ ] `config_loader.py`：新增 `schema_version 3.0`，支持 `defaults`（visibility/chip）、`layers.<L>.<Mod>` 覆盖；保留 `2.0` 解析（D3）。
- [ ] `resolver.py`：实现 §架构提案 6.4 决策（source/lib/auto），解析 `<Layer>` 源码 与 `<Layer>_Libs` 的 `.a`+inc（含 MCAL_Libs 单芯片扁平、BSW_Libs 按模块）。
- [ ] 项目 `SConscript`：对 `mode=lib`/不可见模块 → 注入 `.a` 为链接输入 + 头文件路径。
- [ ] `scons --explain <Mod>`：打印某模块判定为 source/lib 的原因与路径。
- [ ] 校验：`scons -n` 下混合 source(Adc) + lib(Can) 能正确编译+链接。

交付物：`3.0` 版 `config_loader.py`、`resolver.py`、新版 `build.yaml`（Demo_Tc387）。
退出：能在 source/lib 间切换并正确链接 `*_Libs`。

### M3 · 灰度纳入真实层
**目的**：让 v3 真编整仓，而非只 BSW 冒烟。

任务（按层推进，每层一组提交）
- [ ] **MCAL**：纳入 `Tc387/{Adc,Port}`(源码) + `Can`(lib)；处理芯片维。
- [ ] **CDD**：纳入 `CddPwm/CddMotor`(源码+`gen/`) + `CddSent`(lib)。
- [ ] **ASW**：纳入 `VehicleControl`(源码) + `EnergyManagement`(lib)。
- [ ] **Components**：纳入 `Common`、`Mcu/Sbm`（恒源码）。
- [ ] **Integration + 链接**：接 `Integration/`、`Ld/Demo_Tc387.ld`、`Gen/*.c`，链接出 `Demo_Tc387.elf`。
- [ ] 取舍落地（D1=双套并存）：现有模块 `SConscript` 做成 v2/旧**双兼容**，或由通用 builder 接管而旧构建走原路径。

交付物：完整可构建的 `Demo_Tc387/build.yaml` + 各层接入。
退出：`scons -n` 全层构建图通过；`cd Projects/Demo_Tc387 && scons` 旧构建仍可用。

### M4 · 对齐信息安全三视图
**目的**：构建尊重 manifest 的可见性。

任务
- [ ] `workspace.visibility()`：以“工作区是否真有 `<Layer>/<mod>/src`”为可见性事实来源（manifest 决定签出范围）。
- [ ] 用 `manifest-asw.xml` / `manifest-integration.xml` 模拟：隐藏层自动走 `*_Libs`。
- [ ] `scons list` 标注每模块 来源(源码/库) 与 视图。

交付物：可见性解析 + 三视图验证脚本/说明。
退出：切 `manifest-asw` 后，BSW/CDD/MCAL 自动以 `.a` 集成、ASW 源码编译。

### M5 · 收尾与清理
**目的**：补齐配套命令、清理骨架、收敛文档。

任务
- [ ] `scons release`：编源码 → 释放 `.a`+inc 到 `*_Libs`（替/包 `release_libs.bat`）。
- [ ] `scons check`：对接 `misra.json` / `run_gtest`（D4 决定是否本轮做）。
- [ ] `docgen`：接口抽取支持 MCAL/CDD/ASW（非仅 BSW_DIR）。
- [ ] 平台清理：补全/标注 `tasking`；移出未完成 adapter 出默认集合。
- [ ] 文档：更新 `README`/`index.html`/本目录；按 D1 决定是否退役旧构建并给迁移说明。
- [ ] 验收：跑通 §架构提案 §13 全部 5 条。

退出：验收全过；文档收敛。

---

## 4. 工作流并行分解（workstream）

| 流 | 涉及文件 | 里程碑 |
|----|---------|--------|
| 框架核心 | `workspace.py`/`config_loader.py`/`module_builder.py`/`resolver.py`/`report.py` | M1–M2 |
| 目标适配 | `toolchain/*`、`platform/*` | M3、M5 |
| 项目数据 | `Projects/Demo_Tc387/build.yaml`(+`modules/*.yaml`) | M2–M3 |
| 能力插件 | `codegen/*`、`docgen/*`、`remote/*` | M5（docgen 扩层） |
| 配套命令 | `release`/`check` 别名、`misra`/`gtest` 对接 | M5 |
| 文档 | `Docs/build-system/*`、`README`、`index.html` | 每里程碑同步 |

---

## 5. 测试与验证策略

- **L0 静态**：`python -c` 加载 `config_loader`/`resolver` 解析 `build.yaml`，校验 schema/路径。
- **L1 构建图**：stub `tricore-gcc` + `scons -n`，校验每模块编译命令与链接输入正确。
- **L2 冒烟**：有工具链时真编 `Demo_Tc387.elf`。
- **L3 信息安全**：以三套 manifest 分别解析，断言 source/lib 归属符合预期。
- 每里程碑至少过 L0+L1；M3/M5 争取 L2；M4 过 L3。

---

## 6. 回退方案

- 任一里程碑出问题：还原该里程碑提交即可；旧构建 `cd Projects/Demo_Tc387 && scons` 始终可用。
- v3 `build.yaml`(3.0) 与 v2(2.0) 在加载器中共存，不会互相破坏。

---

## 7. 进度勾选表

> 决策已定：**D1=切到 v3（退役旧构建）/ D2=自动发现+显式清单 / D3=schema 3.0 不兼容 2.0 / D4=含 release+check**。
> 已按一次性实现落地（`scons -n` + stub 工具链验证通过；真机编译待有 TriCore 工具链环境）。

- [x] M0 评审 + D1–D4 拍板
- [x] M1 自动发现（`workspace.py`）+ 通用 builder（`module_builder` 按约定）
- [x] M2 `layers/mode` schema（`config_loader` 3.0）+ `resolver`（source/lib + `*_Libs`）
- [x] M3 全层接入（MCAL/CDD/ASW/BSW/Components + Integration → `Demo_Tc387.elf`）
- [x] M4 信息安全：`auto` 按工作区源码可见性决定 source/lib（`resolver`+`workspace`）
- [x] M5 `release`/`check`/`doc` + 清理（删旧构建与每模块 SConscript）+ `scons list/--explain`
- [ ] 真机验收：在 TriCore 工具链环境跑通真实编译/链接（当前仅 dry-run 验证）

---

*本计划随评审与实施滚动更新。先定 §1 决策门，再按 §3 推进。*
