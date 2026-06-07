# ASW_Libs — ASW 层二进制释放区

ASW 模型与项目相关，按项目建 git 子库 **`asw_libs_<Project>`**。但**工作区中库直接位于
本目录根下**（不带项目子目录）——因为拉取某个 `asw_libs_<Project>` 子库时项目已确定。

```
ASW_Libs/                # = git 子库 asw_libs_<Project> 的检出
├── inc/                 # 项目级公共头 (Rte_Type.h ...)
├── VehicleControl/      # inc/ + libVehicleControl.a
└── EnergyManagement/    # inc/ + libEnergyManagement.a
```

仅含对外 `inc/` 头文件与预编译 `.a`，不含源码。由 `scons release` 回灌。
