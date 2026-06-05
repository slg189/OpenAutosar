@echo off
REM ============================================================================
REM setup_env.bat
REM 设置 TriCore 交叉工具链环境变量。请在命令行中以 "call setup_env.bat" 调用,
REM 以便变量保留在当前 shell 会话中。
REM ============================================================================
if "%TRICORE_CC%"==""           set "TRICORE_CC=tricore-gcc"
if "%TRICORE_AR%"==""           set "TRICORE_AR=tricore-ar"
if "%TRICORE_TOOLCHAIN_BIN%"=="" set "TRICORE_TOOLCHAIN_BIN=C:\Tools\tricore\bin"
set "PATH=%TRICORE_TOOLCHAIN_BIN%;%PATH%"
echo [setup] TRICORE_CC=%TRICORE_CC%  TRICORE_AR=%TRICORE_AR%
echo [setup] PATH 已追加: %TRICORE_TOOLCHAIN_BIN%
