# BSW_Libs — BSW 层二进制释放区

BSW 的 `.a` 已把**项目配置代码（ECUC 生成代码等）一并打包**，与项目相关，按项目建 git 子库
**`bsw_libs_<Project>`**。**工作区中库直接位于本目录根下**（不带项目子目录）。

```
BSW_Libs/                # = git 子库 bsw_libs_<Project> 的检出
├── inc/                 # 公共对外头 (EcuM.h ...)
├── Etas/                # inc/ + libRtaOs.a
└── Vector/              # inc/ + libCom.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `release_libs.bat BSW` 回灌。
