# 环境验证 开发计划（host gcc + CI）

| 项 | 内容 |
|----|------|
| 配套报告 | [`ENV_VALIDATION.html`](ENV_VALIDATION.html)（验证方法与结果） |
| 目标 | 让“源码能编 + 框架 include 连通”成为**廉价、可重复、CI 可门禁**的第一道闸，不依赖 TriCore 工具链 |
| 现状 | ✅ 已落地 `toolchain=gcc` + `platform=host` + `scons compile`；本机 gcc 13.3 实测 13/13 源码模块编译通过（exit 0） |
| 定位 | host-gcc 是**第一道闸**（语法/头文件/宏/框架连通）；目标工具链编译链接 + QEMU 集成测试是**第二道闸** |

> 价值已被证明：host-gcc 真编一次，抓到 2 个 `scons -n` 漏掉的真实 bug（动作签名 `env_`、缺各层公共头）。

---

## 1. 已完成（M0）

- [x] `Tools/site_scons/toolchain/gcc.py`：系统 gcc，通用 C 标志（无嵌入式 flag）
- [x] `Tools/site_scons/platform/host.py`：本机架构，不注入目标 CPU flag
- [x] `Projects/<P>/SConscript`：新增 `compile` 别名（只编源码模块为 `.o`，不链接）
- [x] 修复 host-gcc 暴露的 2 个 bug（动作签名 `env`、各层公共头 `<Layer>/inc`）
- [x] `.gitignore`：忽略就地编译产生的 `**/*.o`、`**/*.d`
- [x] 本机实测：`scons compile TOOLCHAIN=gcc PLATFORM=host` → 13 源码模块编译，exit 0

命令：
```bash
scons compile TOOLCHAIN=gcc PLATFORM=host                 # 全部源码模块
scons compile TOOLCHAIN=gcc PLATFORM=host layers=MCAL     # 某层
scons compile TOOLCHAIN=gcc PLATFORM=host only=Crc,Adc    # 某些模块
```

---

## 2. 里程碑

| 里程碑 | 主题 | 估(人日) | 退出条件 |
|--------|------|:---:|---------|
| **M0** | host-gcc 编译验证（✅ 完成） | — | 13/13 源码模块本机编译通过 |
| **M1** | CI 门禁化（✅ 完成） | — | PR 触发 `scons compile gcc/host`，~15s 绿 |
| **M2** | 告警治理 + `-Werror` | 1.5 | 桩代码零告警；CI 开 `-Werror` |
| **M3** | 多项目 / 多配置矩阵 | 1 | 每个 `build.yaml` 都过；`scons list` 快照入 CI |
| **M4** | 目标工具链第二道闸 | 2 | 有 TriCore runner 时跑真机编译/链接 + QEMU |
| **M5** | 与 `check`/`gtest`/MISRA 串联 | 1.5 | `scons check` 在 CI 跑 cppcheck(MISRA)+单测 |

---

## 3. 里程碑明细

### M1 · CI 门禁化
- [ ] 加 CI workflow：`pip install scons pyyaml` → `scons compile TOOLCHAIN=gcc PLATFORM=host`。
- [ ] 失败时输出失败模块与 gcc 报错；PR 必过。
- [ ] 缓存 pip 依赖，控制时长（目标 < 1 min）。
- 交付：CI 配置 + README 徽章。
- 退出：在 PR 上能看到红/绿，且能复现实跑命令。

### M2 · 告警治理 + `-Werror`
- [ ] 用 `-Wall -Wextra` 跑一遍，清理 demo 桩代码告警（未用参数/隐式声明等）。
- [ ] `gcc.py` 增加可选 `STRICT=1` → 注入 `-Werror`；CI 默认开。
- 退出：`scons compile TOOLCHAIN=gcc PLATFORM=host STRICT=1` 零告警通过。

### M3 · 多项目 / 多配置矩阵
- [ ] 支持多个 `Projects/<P>/build.yaml`；CI 矩阵遍历 `PROJECT=`。
- [ ] `scons list` 输出存档为“构建计划快照”，diff 异常时告警（防止误配置）。
- 退出：新增项目自动纳入 host-gcc 验证。

### M4 · 目标工具链第二道闸
- [ ] 在装有 `hightec`/`tasking` 的自托管 runner 上：`scons`（真机编译+链接 ELF）。
- [ ] `scons` 产物经 `Tools/scripts/run_integration_test.py` 在 QEMU 跑集成测试。
- 退出：目标 ELF 构建 + QEMU 冒烟通过（与 host-gcc 互补）。

### M5 · 串联 check / 单测 / MISRA
- [ ] `scons check` 在 CI 调 cppcheck（按 `misra.json` 范围）+ 单元测试（`run_gtest`）。
- [ ] 报告归档到 `Projects/<P>/Reports/`。
- 退出：一条 CI 流水线覆盖 编译(host) → 静态检查 → 单测。

---

## 4. 测试金字塔（定位）

```
            ┌───────────────────────────────┐
   慢 / 真   │  QEMU 集成测试 (目标 ELF)        │  第二道闸 (有 TriCore runner)
            ├───────────────────────────────┤
            │  目标工具链 编译+链接 (hightec)   │  第二道闸
            ├───────────────────────────────┤
   快 / 廉   │  host gcc 编译 (本计划) + 单测    │  第一道闸 (CI 必过, 无需 TriCore)
            ├───────────────────────────────┤
            │  scons list / config 校验 (静态) │  最快
            └───────────────────────────────┘
```

---

## 5. 风险与缓解

| 风险 | 缓解 |
|------|------|
| host gcc 查不出目标相关问题（位宽/对齐/寄存器） | 明确为“第一道闸”；目标工具链 + QEMU 为第二道闸（M4） |
| 就地 `.o`/`.d` 产物污染工作树 | 已 `.gitignore`；后续可让源码模块对象落到 `Obj/`（变体目录）统一管理 |
| 桩代码与真实供应商码差异 | host 验证针对当前源码；接入真实码后同样跑，差异即时暴露 |
| `-Werror` 一刀切阻塞 | 分阶段（M2）：先清告警再开；可按模块豁免 |

---

## 6. 验收标准

1. PR 上 `scons compile TOOLCHAIN=gcc PLATFORM=host` 自动跑且作为门禁（M1）。
2. `STRICT=1`(`-Werror`) 下零告警通过（M2）。
3. 多 `build.yaml` 均纳入矩阵（M3）。
4. 有 TriCore 环境时，目标 ELF + QEMU 集成测试通过（M4）。
5. `scons check`（MISRA+单测）进 CI（M5）。

---

## 7. 进度勾选表

- [x] M0 host-gcc 编译验证（13/13，exit 0；修 2 bug）
- [x] M1 CI 门禁化（`.github/workflows/build-validation.yml`；push/PR 触发）
- [ ] M2 告警治理 + `-Werror`
- [ ] M3 多项目/多配置矩阵
- [~] M4 第二道闸：`run_qemu.py` 已实现真实 QEMU 加载/半主机解析/JUnit（缺 qemu/ELF 记 skipped）；待 TriCore runner 真跑
- [~] M5 **真实 GoogleTest 已接入并在 CI 跑**（FetchContent googletest，实测 `AdcTest` 1/1 通过）；`scons check`(cppcheck/MISRA) 已有入口，串联待完善

> 附：构建框架已**清理未完成 adapter**，仅保留本仓相关且完整的 `toolchain={gcc,hightec}`、`platform={aurix2g,host}`。
> 真实 gtest（CI 必过）+ 真实 qemu runner（自托管）已落地；本计划随实施滚动更新。
