@echo off
set "ROOT=%~dp0.."
set "TOOLS=%ROOT%\Tools"
if exist "%TOOLS%\Offline\tools\windows-x86_64\w64devkit\bin\gcc.exe" set "PATH=%TOOLS%\Offline\tools\windows-x86_64\w64devkit\bin;%PATH%"
if exist "%TOOLS%\.toolcache\cmake-3.28.6-windows-x86_64\bin\cmake.exe" set "PATH=%TOOLS%\.toolcache\cmake-3.28.6-windows-x86_64\bin;%PATH%"
for /r "%TOOLS%\Offline\tools\windows-x86_64\cppcheck" %%I in (cppcheck.exe) do set "PATH=%%~dpI;%PATH%"
for /r "%TOOLS%\Offline\tools\windows-x86_64\doxygen" %%I in (doxygen.exe) do set "PATH=%%~dpI;%PATH%"
call "%TOOLS%\.venv\Scripts\activate.bat"
echo Offline OpenAutosar environment activated.
