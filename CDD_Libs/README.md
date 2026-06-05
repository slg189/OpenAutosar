# CDD_Libs — CDD 层二进制释放区（按项目分子库）

复杂驱动与具体项目强相关，因此 **每个项目一个 git 子库**：

```
CDD_Libs/
└── <Project>/          # git 子库: cdd-libs-<Project>.git
    └── <Cdd>/          # inc/ + lib<Cdd>.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `release_libs.bat CDD`
（内部按 `PROJECT` 释放到 `CDD_Libs\<Project>\`）回灌。
