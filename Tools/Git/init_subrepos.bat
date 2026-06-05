@echo off
REM ============================================================================
REM init_subrepos.bat
REM 将当前 monorepo 的各文件夹拆分/发布为独立 git 子库, 可选在 GitHub 建仓并推送。
REM   - 固定子库: 读取 Tools\Git\repo_map.txt (路径^|仓库名^|分组) —— MCAL 静态源码、
REM               BSW group 各供应商交付库 (BSW_<Vendor>_<Chip>_<Delivery>) 等
REM   - 项目相关子库: 仓库名 <name>_<PROJECT>, 工作区检出到对应目录根:
REM       项目源码 ASW->asw_<P>, CDD->cdd_<P> (ASW/CDD 未按静态/配置分离)
REM       项目库   ASW_Libs->asw_libs_<P>, CDD_Libs->cdd_libs_<P>, BSW_Libs->bsw_libs_<P>
REM               MCAL_Libs->MCAL_<P> (MCAL group 下按项目区分; 已固定单一芯片, 不分 <Chip>)
REM
REM 用法:
REM   Tools\Git\init_subrepos.bat <REMOTE_BASE> [选项]
REM     REMOTE_BASE        远端基址, 如 https://github.com/<owner> 或 git@github.com:<owner>
REM   选项:
REM     --project <name>   项目名 (默认 Demo_Tc387, 决定 *_libs_<name> 仓库名)
REM     --push             初始化并提交后推送
REM     --dry-run          只打印将要执行的动作
REM     --github <owner>   推送前在 GitHub 为各子库建仓 (owner=用户或组织)
REM     --org              指明 <owner> 是组织 (REST API 走 /orgs/<owner>/repos)
REM     --public           GitHub 仓库设为 public (默认 private)
REM
REM GitHub 建仓优先用 gh CLI (需先 `gh auth login`); 无 gh 时回退 REST API,
REM 需设置环境变量 GITHUB_TOKEN (具备 repo 权限的 PAT)。
REM
REM 说明: 每个子库在临时目录 .subrepos\<name> 下初始化(拷贝该文件夹内容)并提交,
REM       避免在工作树内产生嵌套 .git; 不改动当前 monorepo。
REM ============================================================================
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo 用法: %~nx0 ^<REMOTE_BASE^> [--project ^<name^>] [--push] [--dry-run] [--github ^<owner^>] [--org] [--public] 1>&2
    exit /b 1
)
set "REMOTE_BASE=%~1"
set "DO_PUSH="
set "DRY_RUN="
set "GH_OWNER="
set "GH_IS_ORG="
set "GH_VIS=--private"
set "GH_PRIV=true"
if "%PROJECT%"=="" set "PROJECT=Demo_Tc387"

REM ---- 解析选项 ----
:parse
shift
if "%~1"=="" goto parsed
if /I "%~1"=="--push"    ( set "DO_PUSH=1"  & goto parse )
if /I "%~1"=="--dry-run" ( set "DRY_RUN=1"  & goto parse )
if /I "%~1"=="--org"     ( set "GH_IS_ORG=1" & goto parse )
if /I "%~1"=="--public"  ( set "GH_VIS=--public" & set "GH_PRIV=false" & goto parse )
if /I "%~1"=="--github"  ( set "GH_OWNER=%~2" & shift & goto parse )
if /I "%~1"=="--project" ( set "PROJECT=%~2" & shift & goto parse )
goto parse
:parsed

set "ROOT=%CD%"
set "STAGE=%ROOT%\.subrepos"
set "MAP=%ROOT%\Tools\Git\repo_map.txt"
set "BRANCH=main"

if not exist "%MAP%" ( echo 未找到 %MAP% 1>&2 & exit /b 1 )

echo [init] REMOTE_BASE=%REMOTE_BASE%  project=%PROJECT%  push=%DO_PUSH%  dry-run=%DRY_RUN%  github=%GH_OWNER%
echo.

REM ---- 1) 固定子库 (repo_map.txt) ----
for /F "usebackq eol=# tokens=1,2,3 delims=|" %%P in ("%MAP%") do (
    set "P=%%P"
    if not "!P!"=="" call :make_repo "%%P" "%%Q" "%%R"
)

REM ---- 2) 项目相关子库 -> 仓库名 <name>_<PROJECT> (工作区检出到对应目录根) ----
REM      项目源码: ASW / CDD (未按静态/配置分离, 整体与项目相关)
call :make_proj "ASW"       "asw"       "src"
call :make_proj "CDD"       "cdd"       "src"
REM      项目库: ASW / CDD / BSW / MCAL (含配置代码, 全部按项目)
call :make_proj "ASW_Libs"  "asw_libs"  "libs"
call :make_proj "CDD_Libs"  "cdd_libs"  "libs"
call :make_proj "BSW_Libs"  "bsw_libs"  "libs"
REM      MCAL_Libs 归 MCAL group, 按项目区分 (已固定单一芯片): 仓库名 MCAL_<PROJECT>
call :make_proj "MCAL_Libs" "MCAL"      "mcal"

echo.
echo [init] 完成。临时子库位于 %STAGE% (可在确认后删除)。
goto :eof

REM ---------------------------------------------------------------------------
:make_repo
REM %1=源文件夹  %2=仓库名  %3=分组
set "SRC=%~1"
set "NAME=%~2"
set "GROUP=%~3"
set "URL=%REMOTE_BASE%/%NAME%.git"

if not exist "%SRC%" ( echo   [skip] 不存在: %SRC% & goto :eof )

echo   [repo] %SRC%  -^>  %NAME%.git  ^(%GROUP%^)
if defined DRY_RUN goto :eof

REM ---- 可选: GitHub 建仓 ----
if defined GH_OWNER call :create_github "%NAME%"

set "DST=%STAGE%\%NAME%"
if exist "%DST%" rmdir /S /Q "%DST%"
mkdir "%DST%" 2>nul
xcopy "%SRC%\*" "%DST%\" /E /I /Q /Y >nul

pushd "%DST%"
git init -q -b %BRANCH%
git add -A
git -c user.name=ci -c user.email=ci@example.com commit -q -m "Initial import of %NAME% from monorepo"
git remote add origin "%URL%"
if defined DO_PUSH (
    echo     push -^> %URL%
    git push -u origin %BRANCH%
)
popd
goto :eof

REM ---------------------------------------------------------------------------
:make_proj
REM %1=源目录  %2=仓库名前缀  %3=分组 ; 仓库名 = <前缀>_<PROJECT>
if exist "%~1" call :make_repo "%~1" "%~2_%PROJECT%" "%~3"
goto :eof

REM ---------------------------------------------------------------------------
:create_github
REM %1 = 仓库名。优先 gh CLI, 回退 GitHub REST API (需 GITHUB_TOKEN)。
set "RN=%~1"
where gh >nul 2>nul
if not errorlevel 1 (
    echo     [github] gh repo create %GH_OWNER%/%RN% %GH_VIS%
    gh repo create "%GH_OWNER%/%RN%" %GH_VIS% --disable-wiki >nul 2>nul
    goto :eof
)
if "%GITHUB_TOKEN%"=="" (
    echo     [github] 未找到 gh 且未设置 GITHUB_TOKEN, 跳过建仓 ^(请手动创建 %GH_OWNER%/%RN%^) 1>&2
    goto :eof
)
if defined GH_IS_ORG (
    set "API=https://api.github.com/orgs/%GH_OWNER%/repos"
) else (
    set "API=https://api.github.com/user/repos"
)
echo     [github] REST 建仓 %GH_OWNER%/%RN% ^(%API%^)
curl -s -o nul -H "Authorization: token %GITHUB_TOKEN%" -H "Accept: application/vnd.github+json" ^
     "%API%" -d "{\"name\":\"%RN%\",\"private\":%GH_PRIV%}"
goto :eof
