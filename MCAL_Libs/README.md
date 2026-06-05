# MCAL_Libs — MCAL 层二进制释放区

MCAL 的**配置代码与静态代码一并编译链接为库**，与项目相关。**针对项目 MCAL 芯片已固定**
（TC387 与 TC367 不会同时存在），故**只保留该项目的芯片、不再分 `<Chip>` 一级**，直接按模块名组织。
芯片与项目身份编码进 git 子库名：归 **MCAL group**，按项目区分 —— **`MCAL_<Project>`**。
**工作区中库直接位于本目录根下**（不带项目/芯片子目录）。

```
MCAL_Libs/               # = git 子库 MCAL_<Project> 的检出 (MCAL group, 单一芯片)
├── inc/                 # 公共类型 (Std_Types.h)
├── Adc/                 # inc/ + libAdc.a
├── Can/                 # inc/ + libCan.a
└── Port/                # inc/ + libPort.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `release_libs.bat MCAL`（设 `CHIP` 选择释放哪个芯片）回灌。
