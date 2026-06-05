# ASW_Libs — ASW 层二进制释放区（按项目分子库）

ASW 模型与具体项目强相关，因此 **每个项目一个 git 子库**：

```
ASW_Libs/
└── <Project>/          # git 子库: asw-libs-<Project>.git
    ├── inc/            # 项目级公共头文件 (Rte_Type.h ...)
    └── <Model>/        # inc/ + lib<Model>.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `release_libs.bat ASW`
（内部按 `PROJECT` 释放到 `ASW_Libs\<Project>\`）回灌。
