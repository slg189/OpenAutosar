# CDD_Libs — CDD 层二进制释放区

复杂驱动与项目相关，按项目建 git 子库 **`cdd_libs_<Project>`**。**工作区中库直接位于本目录根下**
（不带项目子目录），项目身份体现在子库名中。

```
CDD_Libs/                # = git 子库 cdd_libs_<Project> 的检出
├── CddPwm/              # inc/ + libCddPwm.a
├── CddMotor/            # inc/ + libCddMotor.a
└── CddSent/             # inc/ + libCddSent.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `release_libs.bat CDD` 回灌。
