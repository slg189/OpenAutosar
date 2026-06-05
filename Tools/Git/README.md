# Tools/Git — 多子库 (multi-repo) 创建与维护

本目录提供把 monorepo 按文件夹拆分为独立 git 子库的脚本与清单。

| 文件 | 说明 |
|------|------|
| `repo_map.txt` | 固定子库清单 (路径\|仓库名\|分组) |
| `init_subrepos.bat` | 按 repo_map + 自动遍历项目库, 初始化、(可选)GitHub 建仓并推送 |

## 子库划分

- **源码**: 每个顶层文件夹一个子库 (asw / bsw / cdd / mcal / components / projects / tools / test / docs)
- **共享库**: `mcal_libs` (按芯片, 跨项目)
- **项目库**: `asw_libs_<Project>` / `cdd_libs_<Project>` / `bsw_libs_<Project>`
  —— ASW、CDD、BSW 与项目相关 (BSW 的 `.a` 已把项目配置代码一并打包), **按项目建子库**;
  命名扁平、下划线分隔

## 用法

```bat
REM 预览将创建的子库
Tools\Git\init_subrepos.bat https://github.com/<owner> --dry-run

REM 本地初始化并推送 (远端空仓库需已存在)
Tools\Git\init_subrepos.bat https://github.com/<owner> --push
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
