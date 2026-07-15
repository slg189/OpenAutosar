@echo off
REM 默认打开本地静态检查页面; 传 --run 时保留旧方式直接跑完整 check。
if /I "%~1"=="--run" (
  call "%~dp0oa.bat" check %2 %3 %4 %5 %6 %7 %8 %9
) else (
  call "%~dp0oa.bat" check-ui %*
)
