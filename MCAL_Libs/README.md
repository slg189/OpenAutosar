# MCAL_Libs — MCAL 层二进制释放区

MCAL 的**配置代码与静态代码一并编译链接为库**，与项目相关，按项目建 git 子库
**`mcal_libs_<Project>`**。**工作区中库直接位于本目录根下**（不带项目子目录），
项目身份体现在子库名中；内部仍按芯片再分一级。

```
MCAL_Libs/               # = git 子库 mcal_libs_<Project> 的检出
├── inc/                 # 公共类型 (Std_Types.h)
├── Tc387/               # <Mod>/inc + lib<Mod>.a
└── Tc367/               # <Mod>/inc + lib<Mod>.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `release_libs.bat MCAL`（设 `CHIP`）回灌。
