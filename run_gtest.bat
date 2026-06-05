@echo off
REM ============================================================================
REM run_gtest.bat
REM 构建并运行 GoogleTest 单元测试 (Projects\<PROJECT>\Test\ut), 覆盖率到 coverage_output\。
REM 用法: run_gtest.bat [module]   module 为空则运行全部
REM 注: 单元测试与报告已随工程下沉到 Projects\<PROJECT>\{Test,Reports} (默认 Demo_Tc387)。
REM ============================================================================
setlocal

if "%PROJECT%"=="" set "PROJECT=Demo_Tc387"
set "BUILD_DIR=build_test"
set "COVERAGE_DIR=coverage_output"
set "REPORT_DIR=Projects\%PROJECT%\Reports"
set "MODULE=%~1"
if "%MODULE%"=="" set "MODULE=all"

if not exist "%BUILD_DIR%"    mkdir "%BUILD_DIR%"
if not exist "%COVERAGE_DIR%" mkdir "%COVERAGE_DIR%"
if not exist "%REPORT_DIR%"   mkdir "%REPORT_DIR%"

echo [gtest] 配置构建 (module=%MODULE%) ...
cmake -S Projects\%PROJECT%\Test\ut -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Coverage -DGTEST_MODULE=%MODULE% || goto :err

echo [gtest] 编译 ...
cmake --build "%BUILD_DIR%" -j %NUMBER_OF_PROCESSORS% || goto :err

echo [gtest] 运行测试 ...
ctest --test-dir "%BUILD_DIR%" --output-on-failure --output-junit "..\%REPORT_DIR%\gtest_results.xml" || goto :err

echo [gtest] 采集覆盖率 (需安装 OpenCppCoverage) ...
where OpenCppCoverage >nul 2>nul && (
    OpenCppCoverage --export_type=html:"%COVERAGE_DIR%\html" --working_dir "%BUILD_DIR%" -- ctest --test-dir "%BUILD_DIR%"
) || echo [gtest] 跳过覆盖率 (未找到 OpenCppCoverage)

echo [gtest] 完成。报告: %REPORT_DIR%\gtest_results.xml  覆盖率: %COVERAGE_DIR%\html\index.html
goto :eof

:err
echo [gtest] 失败, 错误码 %errorlevel% 1>&2
exit /b %errorlevel%
