@echo off
setlocal EnableDelayedExpansion

rem Build BqLog Python Wrapper and pack into wheel
rem Reference: build/wrapper/nodejs/build_all_and_pack.bat

set "DIR=%~dp0"
set "PROJECT_ROOT=%DIR%..\..\.."
set "BUILD_LIB_DIR=%PROJECT_ROOT%\build\lib\win64"
set "ARTIFACTS_DIR=%PROJECT_ROOT%\artifacts"
set "WRAPPER_DIR=%PROJECT_ROOT%\wrapper\python"
set "INSTALL_DIR=%PROJECT_ROOT%\install\wrapper_python"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=RelWithDebInfo"

echo ===== Building BqLog Dynamic Library with Python Support [%CONFIG%] =====
pushd "%BUILD_LIB_DIR%"
call dont_execute_this.bat build native msvc OFF OFF ON dynamic_lib %CONFIG%
popd

echo ===== Preparing wheel package =====
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\%CONFIG%"
if not exist "%LIB_PATH%" (
    if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\Release" (
        set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\Release"
    ) else if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\RelWithDebInfo" (
        set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\RelWithDebInfo"
    )
)

echo Copying C Extension to wrapper...
for %%f in ("%LIB_PATH%\_bqlog*.pyd") do (
    copy /Y "%%f" "%WRAPPER_DIR%\src\bq\" >nul
    echo Copied %%~nxf
)

echo ===== Building wheel package =====
pushd "%WRAPPER_DIR%"
py -m pip install --upgrade pip setuptools wheel
py -m pip wheel . -w "%INSTALL_DIR%"
popd

echo ===== Done =====
echo Wheel packages are in: %INSTALL_DIR%
dir "%INSTALL_DIR%\*.whl" 2>nul
