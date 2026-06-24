@echo off
REM ============================================================================
REM repo_metasar.bat
REM 通过 google-repo (manifest) 同步 OpenAutosar 各子仓库 (BSW / MCAL / CDD 等)。
REM 用法: repo_metasar.bat [init^|sync^|status]
REM ============================================================================
setlocal enabledelayedexpansion

if "%MANIFEST_URL%"==""    set "MANIFEST_URL=ssh://git@example.com/autosar/manifest.git"
if "%MANIFEST_BRANCH%"=="" set "MANIFEST_BRANCH=main"
REM 信息安全分级视图 (见 manifests\): default.xml ^| manifest-asw.xml ^| manifest-integration.xml
if "%MANIFEST_NAME%"==""   set "MANIFEST_NAME=default.xml"
set "REPO_DIR=Repo"

REM 选 repo 命令: 优先系统 PATH 上的 repo; 没有则回退到随仓库自带的 Tools\Git\repo.bat
set "REPO=repo"
where repo >nul 2>nul || set "REPO=%~dp0Tools\Git\repo.bat"

set "CMD=%~1"
if "%CMD%"=="" set "CMD=sync"

if /I "%CMD%"=="init" (
    echo [metasar] 初始化 repo manifest: %MANIFEST_URL% ^(%MANIFEST_BRANCH%^) -^> %MANIFEST_NAME%
    if not exist "%REPO_DIR%" mkdir "%REPO_DIR%"
    pushd "%REPO_DIR%"
    call "%REPO%" init -u "%MANIFEST_URL%" -b "%MANIFEST_BRANCH%" -m "%MANIFEST_NAME%"
    popd
    goto :eof
)
if /I "%CMD%"=="sync" (
    echo [metasar] 同步所有子仓库 ...
    pushd "%REPO_DIR%"
    call "%REPO%" sync -j%NUMBER_OF_PROCESSORS%
    popd
    goto :eof
)
if /I "%CMD%"=="status" (
    pushd "%REPO_DIR%"
    call "%REPO%" status
    popd
    goto :eof
)

echo 用法: %~nx0 [init^|sync^|status] 1>&2
exit /b 1
