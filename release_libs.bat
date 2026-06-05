@echo off
REM ============================================================================
REM release_libs.bat
REM 将各层源码模块编译为 .a, 并连同对外头文件(inc\)释放到根 *_Libs\。
REM 信息安全分级释放: 下游(ASW工程师/基础工程师)只拿到二进制库与头文件。
REM
REM 用法:
REM   release_libs.bat [layer]      layer = ASW^|BSW^|CDD^|MCAL^|all   默认 all
REM   set CHIP=Tc387 ^& release_libs.bat MCAL
REM ============================================================================
setlocal enabledelayedexpansion

if "%CHIP%"==""    set "CHIP=Tc387"
if "%PROJECT%"=="" set "PROJECT=Demo_Tc387"
set "LAYER=%~1"
if "%LAYER%"=="" set "LAYER=all"

if /I "%LAYER%"=="MCAL" ( call :release_mcal & goto :done )
if /I "%LAYER%"=="CDD"  ( call :release_cdd  & goto :done )
if /I "%LAYER%"=="ASW"  ( call :release_asw  & goto :done )
if /I "%LAYER%"=="BSW"  ( call :release_bsw  & goto :done )
if /I "%LAYER%"=="all"  ( call :release_mcal & call :release_cdd & call :release_asw & call :release_bsw & goto :done )

echo 用法: %~nx0 [ASW^|BSW^|CDD^|MCAL^|all] 1>&2
exit /b 1

:done
echo [release] 完成 (%LAYER%, chip=%CHIP%)。
goto :eof

REM ---------------------------------------------------------------------------
:copy_headers
REM %1=src_inc_dir  %2=dst_inc_dir
if not exist "%~2" mkdir "%~2"
if exist "%~1\*.h" copy /Y "%~1\*.h" "%~2\" >nul
goto :eof

:build_lib
REM %1=src_dir  %2=module  %3=dst_dir
if not exist "%~3" mkdir "%~3"
if exist "%~1\SConscript" (
    echo   [release] 编译 %~2 ^(source^) -^> %~3\lib%~2.a
    pushd "Projects\%PROJECT%"
    scons only=%~2 chip=%CHIP% release >nul 2>nul || echo   [release] ^(跳过编译, 工具链不可用; 仅同步头文件^)
    popd
)
call :copy_headers "%~1\inc" "%~3\inc"
goto :eof

:release_mcal
REM 源码按芯片 (MCAL\<CHIP>\<Mod>); MCAL_Libs 项目已固定单一芯片, 释放到 MCAL_Libs\<Mod>
REM (芯片身份在 git 子库名中, MCAL group 下按项目区分)。
for %%M in (Adc Can Port) do (
    if exist "MCAL\%CHIP%\%%M" call :build_lib "MCAL\%CHIP%\%%M" "%%M" "MCAL_Libs\%%M"
)
call :copy_headers "MCAL\inc" "MCAL_Libs\inc"
goto :eof

:release_cdd
REM CDD 释放到 CDD_Libs\ (项目身份在子库名 cdd_libs_<PROJECT> 中)
for /D %%D in (CDD\*) do call :build_lib "%%D" "%%~nxD" "CDD_Libs\%%~nxD"
goto :eof

:release_asw
REM ASW 释放到 ASW_Libs\ (项目身份在子库名 asw_libs_<PROJECT> 中)
for /D %%D in (ASW\*) do (
    if /I not "%%~nxD"=="inc" call :build_lib "%%D" "%%~nxD" "ASW_Libs\%%~nxD"
)
call :copy_headers "ASW\inc" "ASW_Libs\inc"
goto :eof

:release_bsw
REM BSW 释放到 BSW_Libs\ (按模块名目录; 供应商/芯片/交付版本在 git 子库名中,
REM 子库位于 BSW group, 命名 BSW_<Vendor>_<Chip>_<Delivery>; .a 已把项目配置代码打包)
call :copy_headers "BSW\inc" "BSW_Libs\inc"
for /D %%D in (BSW\*) do (
    if /I not "%%~nxD"=="inc" (
        call :copy_headers "%%D\inc" "BSW_Libs\%%~nxD\inc"
        if exist "%%D\lib\*.a" copy /Y "%%D\lib\*.a" "BSW_Libs\%%~nxD\" >nul
    )
)
goto :eof
