# Tools/Git — 多子库 (multi-repo) 创建与维护

本目录提供把 monorepo 按文件夹拆分为独立 git 子库的脚本与清单。

| 文件 | 说明 |
|------|------|
| `repo_map.txt` | 固定子库清单 (路径\|仓库名\|分组) |
| `init_subrepos.bat` | 按 repo_map + 自动遍历项目库, 初始化并(可选)推送各子库 |

## 子库划分

- **源码**: 每个顶层文件夹一个子库 (asw / bsw / cdd / mcal / components / projects / tools / test / docs)
- **共享库**: `mcal-libs` / `bsw-libs` (平台级 / 供应商级, 跨项目)
- **项目库**: `asw-libs-<Project>` / `cdd-libs-<Project>` —— ASW、CDD 与项目相关, **按项目建子库**

## 用法

```bat
REM 预览将创建的子库
Tools\Git\init_subrepos.bat ssh://git@example.com/autosar --dry-run

REM 本地初始化并推送 (远端空仓库需先在服务器创建)
Tools\Git\init_subrepos.bat ssh://git@example.com/autosar --push
```

之后用 google-repo 清单 (`manifests/`) 把这些子库按目录组装回工作区:

```bat
set MANIFEST_NAME=default.xml && repo_metasar.bat init
repo_metasar.bat sync
```
