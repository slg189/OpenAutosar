@echo off
setlocal EnableExtensions
chcp 65001 >nul

for %%I in ("%~dp0..\..") do set "ROOT=%%~fI"
set "PROJECT_DIR=Projects/Demo_Tc387"
set "ACTION=%~1"
set "RUN_ROOT=%ROOT%"
set "SUBST_DRIVE="

if "%ACTION%"=="" goto :help
if /I "%ACTION%"=="help" goto :help

call :map_root
pushd "%RUN_ROOT%" || exit /b 1

if /I "%ACTION%"=="env" goto :env
if /I "%ACTION%"=="check-tools" goto :check_tools

if not exist "Tools\.venv\Scripts\scons.exe" (
  echo [oa] Tools\.venv is not ready. Creating offline environment...
  py -3.11 Tools\setup_env.py --offline --no-fetch
  if errorlevel 1 goto :fail
)

call "Tools\activate_offline.bat" >nul

if /I "%ACTION%"=="list" goto :list
if /I "%ACTION%"=="host" goto :host
if /I "%ACTION%"=="build-host" goto :host
if /I "%ACTION%"=="gtest" goto :gtest
if /I "%ACTION%"=="check" goto :check
if /I "%ACTION%"=="doc" goto :doc
if /I "%ACTION%"=="target" goto :target
if /I "%ACTION%"=="build-target" goto :target

echo [oa] Unknown action: %ACTION%
goto :help_after_pushd

:env
py -3.11 Tools\setup_env.py --offline --no-fetch --force
goto :done

:check_tools
py -3 Tools\setup_env.py --check
goto :done

:list
scons list PROJECT=%PROJECT_DIR% %2 %3 %4 %5 %6 %7 %8 %9
goto :done

:host
scons compile PROJECT=%PROJECT_DIR% TOOLCHAIN=gcc PLATFORM=host %2 %3 %4 %5 %6 %7 %8 %9
goto :done

:gtest
py -3.11 Tools\scripts\run_gtest.py --project Demo_Tc387 %2 %3 %4 %5 %6 %7 %8 %9
goto :done

:check
scons check PROJECT=%PROJECT_DIR% TOOLCHAIN=gcc PLATFORM=host %2 %3 %4 %5 %6 %7 %8 %9
goto :done

:doc
scons doc PROJECT=%PROJECT_DIR% TOOLCHAIN=gcc PLATFORM=host %2 %3 %4 %5 %6 %7 %8 %9
goto :done

:target
scons PROJECT=%PROJECT_DIR% %2 %3 %4 %5 %6 %7 %8 %9
goto :done

:help_after_pushd
popd
:help
echo Usage: oa.bat ^<action^> [extra scons/script args]
echo.
echo Actions:
echo   env          Recreate offline Python 3.11 venv
echo   check-tools  Verify bundled tools
echo   list         Show build plan
echo   host         Host compile with bundled GCC
echo   gtest        Run GoogleTest via bundled CMake/CTest/GTest
echo   check        Run cppcheck/MISRA entry plus unit tests
echo   doc          Generate project docs
echo   target       Build default target toolchain from build.yaml
echo.
echo Examples:
echo   oa.bat list
echo   oa.bat host only=Adc
echo   oa.bat gtest --module Adc
exit /b 0

:done
set "RC=%ERRORLEVEL%"
popd
if defined SUBST_DRIVE subst %SUBST_DRIVE% /d >nul 2>nul
exit /b %RC%

:fail
set "RC=%ERRORLEVEL%"
popd
if defined SUBST_DRIVE subst %SUBST_DRIVE% /d >nul 2>nul
exit /b %RC%

:map_root
for %%D in (O: P: Q: R: S: T:) do (
  if not exist %%D\NUL (
    subst %%D "%ROOT%" >nul 2>nul
    if not errorlevel 1 (
      set "RUN_ROOT=%%D\"
      set "SUBST_DRIVE=%%D"
      goto :eof
    )
  )
)
goto :eof
