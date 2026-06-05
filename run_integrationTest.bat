@echo off
REM ============================================================================
REM run_integrationTest.bat
REM 在 QEMU 上运行集成测试 (依据 TestPlatform_UintTest_QEMU_Test.json 配置)。
REM 用法: run_integrationTest.bat [project]   默认 Demo_Tc387
REM ============================================================================
setlocal

set "PROJECT=%~1"
if "%PROJECT%"=="" set "PROJECT=Demo_Tc387"
set "CONFIG=TestPlatform_UintTest_QEMU_Test.json"
set "REPORT_DIR=Projects\%PROJECT%\Reports"

if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%"

echo [itest] 构建工程 %PROJECT% ...
pushd "Projects\%PROJECT%"
scons -j %NUMBER_OF_PROCESSORS%
set "RC=%errorlevel%"
popd
if not "%RC%"=="0" goto :err

set "ELF=Projects\%PROJECT%\Out\%PROJECT%.elf"
if not exist "%ELF%" (
    echo [itest] 未找到 ELF: %ELF% 1>&2
    exit /b 1
)

echo [itest] 在 QEMU 中运行集成测试 (config=%CONFIG%) ...
python Projects\%PROJECT%\Test\Projects\run_qemu.py --config "%CONFIG%" --elf "%ELF%" --report "%REPORT_DIR%\integration_%PROJECT%.xml" || goto :err

echo [itest] 完成。报告: %REPORT_DIR%\integration_%PROJECT%.xml
goto :eof

:err
echo [itest] 失败, 错误码 %errorlevel% 1>&2
exit /b %errorlevel%
