@echo off
REM ============================================================================
REM init_subrepos.bat
REM 将当前 monorepo 的各文件夹拆分/发布为独立 git 子库并推送到远端。
REM   - 固定子库: 读取 Tools\Git\repo_map.txt (路径^|仓库名^|分组)
REM   - 项目库:   自动遍历 ASW_Libs\* 与 CDD_Libs\* 各项目, 生成
REM               asw-libs-<project> / cdd-libs-<project>
REM
REM 用法:
REM   Tools\Git\init_subrepos.bat <REMOTE_BASE> [--push] [--dry-run]
REM     REMOTE_BASE  远端基址, 例如 ssh://git@example.com/autosar
REM     --push       初始化并提交后推送 (默认仅本地准备, 不推送)
REM     --dry-run    只打印将要执行的动作
REM
REM 说明: 为避免在工作树内产生嵌套 .git, 每个子库在临时目录 .subrepos\<name>
REM       下初始化(拷贝该文件夹内容), 提交后推送; 不改动当前 monorepo。
REM       远端空仓库需预先在服务器创建 (GitHub 需先建仓; GitLab 可在 push 时自动建)。
REM ============================================================================
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo 用法: %~nx0 ^<REMOTE_BASE^> [--push] [--dry-run] 1>&2
    exit /b 1
)
set "REMOTE_BASE=%~1"
set "DO_PUSH="
set "DRY_RUN="
for %%A in (%*) do (
    if /I "%%~A"=="--push"    set "DO_PUSH=1"
    if /I "%%~A"=="--dry-run" set "DRY_RUN=1"
)

set "ROOT=%CD%"
set "STAGE=%ROOT%\.subrepos"
set "MAP=%ROOT%\Tools\Git\repo_map.txt"
set "BRANCH=main"

if not exist "%MAP%" ( echo 未找到 %MAP% 1>&2 & exit /b 1 )

echo [init] REMOTE_BASE=%REMOTE_BASE%  push=%DO_PUSH%  dry-run=%DRY_RUN%
echo.

REM ---- 1) 固定子库 (repo_map.txt) ----
for /F "usebackq eol=# tokens=1,2,3 delims=|" %%P in ("%MAP%") do (
    set "P=%%P"
    if not "!P!"=="" call :make_repo "%%P" "%%Q" "%%R"
)

REM ---- 2) 项目库: ASW_Libs\<proj>, CDD_Libs\<proj> ----
if exist "ASW_Libs" for /D %%D in (ASW_Libs\*) do (
    call :make_repo "ASW_Libs\%%~nxD" "asw-libs-%%~nxD" "libs"
)
if exist "CDD_Libs" for /D %%D in (CDD_Libs\*) do (
    call :make_repo "CDD_Libs\%%~nxD" "cdd-libs-%%~nxD" "libs"
)

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
