# Test

| 目录 | 说明 | 入口 |
|------|------|------|
| `ut/` | GoogleTest 单元测试（host 上对可主机编译的模块）。`CMakeLists.txt` 用 FetchContent 拉 googletest，编译被测 `.c` + 测试 `.cpp` | `python Tools/scripts/run_gtest.py [--module Adc]` |
| `Projects/` | QEMU 集成测试运行脚本（`run_qemu.py`：加载目标 ELF，解析半主机输出，写 JUnit；缺 qemu/ELF 时记 skipped） | `python Tools/scripts/run_integration_test.py` |
| `QAC/` | QAC / MISRA 静态分析规则集 | `scons check`（cppcheck，范围见 `misra.json`） |

- 单元测试 host 即可跑（CI 已接入，见根 `.github/workflows/build-validation.yml`）。
- 集成测试需 TriCore 工具链构建 ELF + `qemu-system-tricore`（自托管 runner / 本地）。
- 报告输出到 `Projects/<P>/Reports/`。
