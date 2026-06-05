# Tools/Git — 多子库 (multi-repo) 创建与维护

本目录提供把 monorepo 按文件夹拆分为独立 git 子库的脚本与清单。

| 文件 | 说明 |
|------|------|
| `repo_map.txt` | 固定子库清单 (路径\|仓库名\|分组) |
| `init_subrepos.bat` | 按 repo_map + 自动遍历项目库, 初始化、(可选)GitHub 建仓并推送 |

## 子库划分

- **共享源码**: `mcal` / `components` / `projects` / `tools` 各一个子库。
  `Test` / `Docs` / `Reports` 已随工程下沉到 `Projects/<P>/`，属 `projects` 子库。
- **BSW 供应商交付 (BSW group)**: `BSW/` 不再分 `Vector`/`Etas` 目录，仅按模块名 (`Os`/`Com`/…) 组织;
  供应商/芯片/交付版本编码进子库名 `BSW_<Vendor>_<Chip>_<Delivery>` (如 `BSW_Vector_TC387_CBDxxxxxx`、
  `BSW_Etas_TC387_RtaOs`)，**每家供应商一个交付库**，各自检出到 `BSW/<模块>`，共同组成扁平的 `BSW/` 树。
  这些条目在 `repo_map.txt` 中以 `BSW/<模块>|<库名>|bsw` 声明，交付版本号按实际交付改名。
- **项目源码**: `asw_<Project>` / `cdd_<Project>` (ASW/CDD 未按静态/配置分离, 整体与项目相关)。
- **项目库**: `asw_libs_<Project>` / `cdd_libs_<Project>` / `bsw_libs_<Project>` / `mcal_libs_<Project>`
  —— 与项目相关 (BSW/MCAL 的 `.a` 已把项目配置代码一并打包), **按项目建子库**; 库目录同样按模块名、不分供应商;
  命名扁平、下划线分隔。**工作区中这些库直接检出到 `<Layer>_Libs/` 根**（不带项目子目录）——
  拉取某个 `*_libs_<Project>` 时项目已确定，项目身份只在子库名中。

## 用法

```bat
REM 预览将创建的子库 (--project 决定 *_libs_<name> 仓库名, 默认 Demo_Tc387)
Tools\Git\init_subrepos.bat https://github.com/<owner> --project Demo_Tc387 --dry-run

REM 本地初始化并推送 (远端空仓库需已存在)
Tools\Git\init_subrepos.bat https://github.com/<owner> --project Demo_Tc387 --push
```

## 在 GitHub 上建仓 + 推送

`init_subrepos.bat` 可在推送前自动创建 GitHub 仓库：优先用 **gh CLI**，无 gh 时回退 **REST API**（需 `GITHUB_TOKEN`）。

### 方式 A：gh CLI（推荐）

```bat
REM 1) 安装并登录 (一次性): https://cli.github.com/
gh auth login

REM 2) 个人账户: 私有仓, 建仓 + 推送
Tools\Git\init_subrepos.bat https://github.com/<your-user> --github <your-user> --push

REM    组织账户加 --org; 公开仓加 --public
Tools\Git\init_subrepos.bat https://github.com/<org> --github <org> --org --public --push
```

### 方式 B：REST API（无 gh 时）

```bat
REM 1) 生成具备 repo 权限的 PAT, 设为环境变量
set GITHUB_TOKEN=ghp_xxxxxxxxxxxxxxxx

REM 2) 建仓 + 推送 (个人账户用 /user/repos; 组织加 --org 用 /orgs/<org>/repos)
Tools\Git\init_subrepos.bat https://github.com/<owner> --github <owner> --push
```

> 单独手动建一个仓库也可：
> `gh repo create <owner>/asw_libs_Demo_Tc387 --private`
> 或 `curl -H "Authorization: token %GITHUB_TOKEN%" https://api.github.com/user/repos -d "{\"name\":\"asw_libs_Demo_Tc387\",\"private\":true}"`

## 组装工作区 (google-repo)

```bat
set MANIFEST_NAME=default.xml && repo_metasar.bat init
repo_metasar.bat sync
```
