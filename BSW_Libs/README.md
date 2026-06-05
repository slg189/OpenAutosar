# BSW_Libs — BSW 层二进制释放区（按项目分子库）

BSW 的 `.a` 已把**项目配置代码（ECUC 生成代码等）一并打包**，与具体项目强相关，
因此 **每个项目一个 git 子库**：

```
BSW_Libs/
└── <Project>/          # git 子库: bsw_libs_<Project>.git
    ├── inc/            # 公共对外头 (EcuM.h ...)
    ├── Etas/inc + libRtaOs.a
    └── Vector/inc + libCom.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `release_libs.bat BSW`
（内部按 `PROJECT` 释放到 `BSW_Libs\<Project>\`）回灌。
