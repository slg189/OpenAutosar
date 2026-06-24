@echo off
REM ============================================================================
REM Tools\Git\repo.bat — Windows 下 google-repo 启动器封装
REM
REM google-repo 的 `repo` 本体是个 Python 脚本 (Tools\Git\repo), Windows 上
REM 不能直接执行, 需用 python 调起。本封装把本目录的 repo 脚本交给 python 运行,
REM 使得 `repo init/sync/status ...` 在 Windows 上可用 (无需另装 repo)。
REM
REM 用法: 把本目录 (Tools\Git) 加入 PATH 后, 直接 `repo <子命令> ...` 即可;
REM       或显式调用 `Tools\Git\repo.bat <子命令> ...`。
REM 依赖: 系统已装 Python 3.6+ 与 Git, 且均在 PATH 上。
REM ============================================================================
setlocal

REM 选 python 解释器 (优先 py 启动器, 退到 python)
set "PY=py -3"
where py >nul 2>nul || set "PY=python"

REM 本封装同目录下的 repo 启动脚本
set "REPO_LAUNCHER=%~dp0repo"
if not exist "%REPO_LAUNCHER%" (
    echo [repo.bat] 未找到 repo 启动脚本: %REPO_LAUNCHER% 1>&2
    echo [repo.bat] 可重新下载: curl https://storage.googleapis.com/git-repo-downloads/repo -o "%REPO_LAUNCHER%" 1>&2
    exit /b 1
)

%PY% "%REPO_LAUNCHER%" %*
exit /b %ERRORLEVEL%
