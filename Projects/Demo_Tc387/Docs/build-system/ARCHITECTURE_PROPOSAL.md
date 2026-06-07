# OpenAutosar 构建系统架构提案 — v3（统一·简单·平台化）

| 项 | 内容 |
|----|------|
| 状态 | **已实施**（决策 D1 切到 v3 / D2 自动发现+显式清单 / D3 schema 3.0 / D4 含 release+check） |
| 范围 | 构建系统目标架构；旧构建已退役，仓库根 `scons` 即 v3。本文保留作设计依据与原框架对比 |
| 关联 | 实施进度见 [`DEVELOPMENT_PLAN.md`](DEVELOPMENT_PLAN.md) §7；命令/落地见本目录 `README.md` |
| 目标读者 | 平台/集成负责人、构建维护者、各层工程师 |

> 一句话主张：**用 v2 的“数据驱动 + 插件化”架构，承载原框架的“领域模型（分层 / 源码-库双形态 / 信息安全 *_Libs）”，再叠加“约定优于配置”把用户与维护者的改动面压到最小。**

---

## 1. 背景与现状

仓库当前**并存两套**构建：

| | 原框架（fallback） | v2 框架（已落地根目录） |
|---|---|---|
| 入口 | 每项目 `Projects/<P>/SConstruct` | 仓库根 `SConstruct`（薄） |
| 公共逻辑 | `Tools/Scons/build_helpers.py` | `Tools/site_scons/**`（平台包） |
| 配置 | `module_config.py`（Python 字典） | `Projects/<P>/build.yaml`（数据 + schema 校验） |
| 目标抽象 | 工具链/平台写死（tricore-gcc/AURIX） | 工具链/平台/代码生成/文档/远程 插件化 |
| 领域建模 | ✓ 五层 + source/lib + `*_Libs` 信息安全 + MCAL 芯片维 + Integration + Gen/Ld | ✗ 仅 bsw/mcal/components 三桶，缺 ASW/CDD、缺“只链接的库”、缺信息安全 |
| 代码生成/文档/远程 | ✗ 无 | ✓ 有 |
| 能否真编整仓 | ✓ 能 | ✗ 当前仅 BSW 冒烟（Crc/Os/Com） |

**结论**：v2 赢在“架构 / 平台 / 能力”，原框架赢在“领域模型 / 成熟度 / 上手”。两者都不是完整答案。

---

## 2. 痛点（为什么要改）

1. **配置面割裂**：原框架改 Python（脆、无校验、不友好）；v2 改 YAML（好），但表达不了本仓架构。
2. **v2 缺领域模型**：无 ASW/CDD 层、无“预编译只链接”、无信息安全 `*_Libs`、MCAL 芯片只能 `name: "Tc387/Adc"` 凑。
3. **每模块一份 SConscript**：v2 缺则 fail-fast；70+ BSW 模块 = 70 份近似文件，迁移负担巨大。
4. **不够平台**：原框架每项目一份 SConstruct，逻辑住在项目里、跨项目复制。
5. **骨架污染**：v2 多个未完成 adapter（ghs/iar/tasking、r5/r52p/yuntu）会误导用户。

---

## 3. 目标与非目标

**目标**
- G1 用户只改一份 `build.yaml`，且**通常只写例外**（不枚举全部模块）。
- G2 一套框架服务所有项目/芯片/工具链/供应商；**新项目=新 `build.yaml`，新目标=新 adapter**。
- G3 原生表达本仓领域：五层 + 每模块 `source|lib` + 信息安全 `*_Libs` + MCAL 芯片维。
- G4 保留 v2 的代码生成 / 文档 / 远程 能力，但**右尺寸**（默认零配置可用）。
- G5 零项目脚本：项目目录只剩数据。

**非目标**
- N1 不换构建工具（仍 SCons，理由见 §12）。
- N2 不在本提案动现有两套构建（仅文档）。
- N3 不追求一次性支持所有芯片/工具链，只完成本仓所需 + 留好插件位。

---

## 4. 设计原则

1. **约定优于配置**：能扫描推断的不让用户写。
2. **数据优于代码**：项目配置是声明式数据 + schema 校验。
3. **一个统一概念**：所有层、所有模块用同一个 `mode: source|lib` 模型。
4. **可见即可编，不可见即链接**：构建跟随 manifest 的信息安全可见性。
5. **可发现**：隐式推断必须能被 `scons list/--explain` 打印出来。
6. **渐进可回退**：与旧构建并存，按模块灰度切换。

---

## 5. 目标架构总览

```
仓库根/
├── SConstruct                  # 薄入口 (不含项目逻辑)
├── Tools/site_scons/         # ★ 平台框架 (维护者唯一改动面)
│   ├── workspace.py            #   工作区/模块自动发现 + 可见性解析
│   ├── config_loader.py        #   build.yaml -> 强类型 + 校验
│   ├── module_builder.py       #   通用按约定编译 (取代每模块 SConscript)
│   ├── resolver.py             #   source/lib + *_Libs + 信息安全 自动决策
│   ├── toolchain/  platform/   #   插件: 目标抽象
│   ├── codegen/  docgen/  remote/  #  插件: 能力 (可选)
│   └── report.py               #   scons list / --explain 输出
└── Projects/<P>/               # ★ 项目 = 纯数据 (用户改动面)
    ├── build.yaml              #   唯一主配置 (只写例外)
    ├── Cfg/ Gen/ Ld/ Obj/ Out/ #   配置/生成码/链接脚本/产物
    └── (可选) modules/*.yaml   #   仅特殊模块的 override
```

两个**最小改动面**：用户只碰 `Projects/<P>/build.yaml`；维护者只碰 `Tools/site_scons/**`。

---

## 6. 核心设计

### 6.1 统一数据模型：`layers` + `mode`（取代 bsw/mcal/components 三桶）

```yaml
# Projects/Demo_Tc387/build.yaml (目标形态)
schema_version: "3.0"
project: { name: Demo, derivative: Tc387 }      # 产物 Demo_Tc387.elf
target:  { toolchain: hightec, platform: AURIX2G, build_type: Release }

defaults:
  visibility: auto        # auto=按工作区可见性决定 source/lib; 也可强制 source|lib
  chip: Tc387             # MCAL 等按芯片的层默认芯片

# 只写“例外”，其余模块由自动发现 + defaults 接管
layers:
  MCAL:
    Can:  { mode: lib }                 # 这个用预编译库
    Adc:  { cflags: [-O3] }             # 仅覆盖编译选项
  ASW:
    EnergyManagement: { mode: lib }
  BSW:
    Com:  { extra_defines: [COM_FD] }
  # MCAL/Adc(源码默认)、CDD/*、ASW/VehicleControl、BSW/Os|Crc、Components/* 全部自动纳入

codegen: { enabled: true, generators: [ { name: demo_echo, type: custom, cmd: "echo gen" } ] }
docs:    { enabled: true }
remote:  { enabled: false }
```

一个 `mode: source|lib` 同时解决：v2 缺 ASW/CDD 层、缺“只链接的库”、缺信息安全；并消除原框架的 Python 配置脆弱。

### 6.2 模块自动发现（约定）

- **模块 = 一个带 `inc/`（可含 `src/`）的目录**。
- 框架扫描各层根（`ASW/ CDD/ MCAL/<chip>/ BSW/ Components/`）自动登记模块。
- `build.yaml` 不再枚举模块，只声明**偏差**（关掉谁、强制 lib、加 flag/define）。
- 默认全部启用；`enabled: false` 显式关闭。

> 用户从“枚举几十个模块”变成“只写几条例外”。这是“简单好用”的最大杠杆。

### 6.3 取消每模块 SConscript（通用 builder + 可选 override）

- 现状：v2 每模块要一份近似 `SConscript`，缺则 fail。
- 目标：`module_builder` 按约定（`src/*.c|*.S`、`inc/`、`Gen/<mod>/`）**直接编译**，无需每模块脚本。
- 仅“特殊”模块（额外源目录、特殊 flag、非常规布局）放一份**可选** `Projects/<P>/modules/<Mod>.yaml` 或模块内 `module.yaml` 覆盖。
- **直接消灭 Phase-2 里“为 70+ 模块写/校 SConscript”那一整类工作。**

### 6.4 source/lib + 信息安全 自动决策（`resolver.py`）

决策输入：① `build.yaml` 的显式 `mode`；② 工作区里该模块**源码是否可见**（manifest 决定）；③ `defaults.visibility`。

```
resolve(layer, module):
    if module.mode == 'lib':            return LINK  <Layer>_Libs/.../lib<Mod>.a + inc
    if module.mode == 'source':         return COMPILE <Layer>/.../src
    # visibility == auto:
    if  <Layer>/<module>/src 可见:       return COMPILE
    elif <Layer>_Libs/<module> 存在:     return LINK
    else:                               FAIL("源码不可见且无释放库")
```

- LINK 时自动解析 `<Layer>_Libs` 的 `.a` 与 `inc`（含 MCAL_Libs 单芯片扁平、BSW_Libs 按模块）。
- 与 `manifests/*.xml`（default/asw/integration 三视图）天然对齐：ASW 工程师只可见 ASW 源码、其余自动走 `*_Libs`。
- 与 `release_libs.bat`(旧) 打通：由 `scons release`（编源码 → 释放 `*_Libs`）取代。

### 6.5 项目 = 纯数据（零项目脚本）

把当前 v2 还残留的 `Projects/<P>/SConscript` 逻辑上收进框架；项目目录只剩 `build.yaml`(+`Cfg/Gen/Ld`)。**加一个项目 = 加一份 `build.yaml`。**

### 6.6 工具链/平台/代码生成/文档/远程：沿用 v2 插件，右尺寸

- 保留 `toolchain/ platform/ codegen/ docgen/ remote/` 插件机制（这是平台化的核心）。
- **完成本仓所需**：`hightec`/`tasking` 工具链、`AURIX2G` 平台（已起步）。
- **清理骨架**：未实现的 adapter 要么补全、要么从默认集合移除并标注“实验性”，避免误导。
- 代码生成/文档/远程均**可选**，默认关；普通项目零配置 `scons` 即可编。

### 6.7 可发现性（把隐式变可见）

```
scons list            # 打印自动发现的模块 + 解析后的 mode/可见性/来源
scons --explain Adc   # 解释某模块为何 source/lib、include/lib 路径、生效 flag
scons graph           # 依赖图 (复用 docgen)
```

---

## 7. 模块解析算法（伪码）

```python
def plan(build_yaml, workspace):
    cfg = load_and_validate(build_yaml)              # schema 校验 + 友好报错
    modules = workspace.discover(cfg.layers_roots)   # 6.2 自动发现
    plan = []
    for m in modules:
        ov   = cfg.override_for(m)                   # build.yaml 里的例外
        if ov and ov.enabled is False: continue
        mode = resolver.resolve(m, ov, cfg.defaults, workspace)   # 6.4
        rule = ov.merge(module_yaml(m))              # 6.3 可选 override
        plan.append(BuildItem(m, mode, rule))
    return plan      # scons list 即打印它
```

---

## 8. 与现有资产的对接

| 现有资产 | 对接方式 |
|---|---|
| `manifests/{default,asw,integration}.xml` | 决定模块源码可见性 → `resolver` 的 `auto` 决策输入 |
| `ASW_Libs/CDD_Libs/BSW_Libs/MCAL_Libs` | `mode=lib`/不可见时的链接来源（路径规则已在原 `build_helpers` 验证过） |
| `release_libs.bat`(旧) | 映射为 `scons release`：编源码 → 释放 `.a`+inc 到 `*_Libs` |
| `Projects/<P>/{Cfg,Gen,Ld}` | 代码生成产物入 `Gen/`；`Ld/<P>.ld` 由 `build.yaml.linker.script` 指定 |
| `misra.json` / `run_gtest` / QAC | 不变；可加 `scons check` 调用 |

---

## 9. 迁移路线（与旧构建的取舍）

> 与旧构建共用模块脚本的取舍方式，待评审（见 §14 决策项 D1）。下列为推荐的**双套并存 + 按层灰度**。

- **M0（本提案）**：文档评审。零改动。
- **M1 自动发现 + 通用 builder**：实现 `workspace.discover` + `module_builder` 按约定编译；BSW 三模块去掉显式 SConscript 验证。
- **M2 统一 `layers/mode` schema + `resolver`**：`config_loader` 支持 `layers`，接入 `*_Libs` 链接与 `auto` 可见性。
- **M3 灰度纳入真实层**：按 `MCAL → CDD → ASW → Components` 顺序纳入；每纳入一层跑 `scons list` + 构建验证；旧构建保持可用。
- **M4 对齐信息安全三视图**：用 `manifest-asw/integration` 验证“源码不可见自动走 `*_Libs`”。
- **M5 收尾**：`scons release`/`check`/`doc`；按 D1 决定是否退役旧构建。

每一步都可独立提交、独立回退。

---

## 10. 兼容与回退

- 全程**保留** `Projects/<P>/SConstruct + build_helpers.py` 作为回退，直到 M5 验收通过。
- 新模型 `build.yaml` 用 `schema_version: "3.0"`，与现 v2 `2.0` 共存于加载器。
- 任一步出问题：`cd Projects/<P> && scons` 走旧构建。

---

## 11. 风险与缓解

| 风险 | 缓解 |
|---|---|
| 自动发现是“隐式魔法”，排错难 | `scons list/--explain` 把解析结果显式化；允许显式 `mode` 兜底 |
| 约定不统一的历史模块（非 `src/inc` 布局） | 可选 `module.yaml` override；扫描器对不合约定者告警而非静默 |
| 信息安全误判（把该隐藏的当源码编） | `auto` 以“工作区是否真有源码”为准，由 manifest 控制；CI 用三视图验证 |
| 平台 adapter 未完成被误用 | 默认集合只含已完成项，其余标 experimental 并在 `scons list` 提示 |
| 迁移期两套并存的认知成本 | 文档 + `README` 明确“根 scons=v3 / 项目内 scons=旧”，M5 后收敛 |

---

## 12. 备选方案（评估后不选）

- **换 CMake / Bazel**：本仓已重度投资 SCons + Python 插件；SCons 与 AUTOSAR 的动态代码生成/依赖契合好；换工具收益不明确、扰动大。**不推荐现在换**，但 `compile_commands.json` 已产出，未来若需可平滑评估。
- **保持 v2 原样仅做 Phase 2（纯搬运）**：能跑通但不解决“每模块脚本/枚举”负担，长期改动面仍大。本提案在同等迁移量下顺手简化，更值。

---

## 13. 验收标准（M5）

1. `scons` 根目录零额外配置即可编出 `Demo_Tc387.elf`（全真实层）。
2. `build.yaml` 仅写例外即可开关任意模块 / 切 source|lib。
3. 删除某模块的源码可见性（切 `manifest-asw`）后，该模块**自动**改走 `*_Libs/.a`。
4. 无需为常规模块编写 SConscript；新增模块放进目录即被发现。
5. `scons list/--explain/doc/release` 均可用；旧构建仍可回退。

---

## 14. 待决问题（请评审拍板）

- **D1 旧构建取舍**：双套并存（推荐）/ 切到 v3 退役旧构建 / 不同文件名并存。
- **D2 自动发现 vs 显式清单**：默认全发现+写例外（推荐）/ 仍要求显式 enable。
- **D3 schema 版本**：`build.yaml` 升 `3.0`（`layers/mode`）是否接受不兼容 2.0。
- **D4 范围**：本轮是否一并完成 `scons release/check`（与 misra/gtest 对接），还是只做编译链路。

---

*本提案为草案，欢迎逐条批注。评审通过后再决定是否按 §9 路线实施（即“升级版 Phase 2”）。*
