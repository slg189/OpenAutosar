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
for %%M in (Adc Can Port) do (
    if exist "MCAL\%CHIP%\%%M" call :build_lib "MCAL\%CHIP%\%%M" "%%M" "MCAL_Libs\%CHIP%\%%M"
)
call :copy_headers "MCAL\inc" "MCAL_Libs\inc"
goto :eof

:release_cdd
for /D %%D in (CDDs\*) do call :build_lib "%%D" "%%~nxD" "CDD_Libs\%%~nxD"
goto :eof

:release_asw
for /D %%D in (ASWs\*) do (
    if /I not "%%~nxD"=="inc" call :build_lib "%%D" "%%~nxD" "ASW_Libs\%%~nxD"
)
call :copy_headers "ASWs\inc" "ASW_Libs\inc"
goto :eof

:release_bsw
REM BSW 通常由供应商以 .a 交付, 此处仅同步头文件与现成库
call :copy_headers "BSW\inc"        "BSW_Libs\inc"
call :copy_headers "BSW\Etas\inc"   "BSW_Libs\Etas\inc"
call :copy_headers "BSW\Vector\inc" "BSW_Libs\Vector\inc"
if exist "BSW\Etas\lib\*.a"   copy /Y "BSW\Etas\lib\*.a"   "BSW_Libs\Etas\"   >nul
if exist "BSW\Vector\lib\*.a" copy /Y "BSW\Vector\lib\*.a" "BSW_Libs\Vector\" >nul
goto :eof
