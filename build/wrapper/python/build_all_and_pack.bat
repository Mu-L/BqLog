@echo off
setlocal EnableDelayedExpansion

rem Build BqLog Python Wrapper and pack into wheel (Windows)
rem Step 1: Build native .pyd with Python support
rem Step 2: Run wrapper/python/CMakeLists.txt (version sync + artifacts staging)
rem Step 3: Copy native lib into staged artifacts
rem Step 4: Build wheel from staged artifacts

set "DIR=%~dp0"
set "PROJECT_ROOT=%DIR%..\..\.."
set "BUILD_LIB_DIR=%PROJECT_ROOT%\build\lib\win64"
set "ARTIFACTS_DIR=%PROJECT_ROOT%\artifacts"
set "WRAPPER_DIR=%PROJECT_ROOT%\wrapper\python"
set "INSTALL_DIR=%PROJECT_ROOT%\install\wrapper_python"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=RelWithDebInfo"

rem Check if first arg is an arch (arm64) or a config
set "ARCH=native"
if /i "%CONFIG%"=="arm64" (
    set "ARCH=arm64"
    set "CONFIG=%~2"
    if "!CONFIG!"=="" set "CONFIG=RelWithDebInfo"
)

rem ===== Step 1: Build native library =====
echo ===== Building BqLog Dynamic Library with Python Support [%CONFIG%] [%ARCH%] =====
pushd "%BUILD_LIB_DIR%"
call dont_execute_this.bat build %ARCH% msvc OFF OFF ON dynamic_lib %CONFIG%
if %ERRORLEVEL% neq 0 (
    popd
    echo ERROR: Native library build failed!
    exit /b 1
)
popd

rem ===== Step 2: Run wrapper CMake (version sync + staging) =====
echo ===== Running wrapper/python CMake (version sync + staging) =====
set "WRAPPER_BUILD_DIR=%DIR%wrapperProj"
if exist "%WRAPPER_BUILD_DIR%" rd /s /q "%WRAPPER_BUILD_DIR%"
md "%WRAPPER_BUILD_DIR%"
pushd "%WRAPPER_BUILD_DIR%"
cmake "%WRAPPER_DIR%" || exit /b 1
cmake --build . --target install || exit /b 1
popd

rem ===== Step 3: Copy native lib into install dir =====
echo ===== Copying native library to wheel source =====
set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\%CONFIG%"
if not exist "%LIB_PATH%" (
    if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\Release" (
        set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\Release"
    ) else if exist "%ARTIFACTS_DIR%\dynamic_lib\lib\RelWithDebInfo" (
        set "LIB_PATH=%ARTIFACTS_DIR%\dynamic_lib\lib\RelWithDebInfo"
    )
)

for %%f in ("%LIB_PATH%\_bqlog*.pyd") do (
    copy /Y "%%f" "%INSTALL_DIR%\src\bq\" >nul
    copy /Y "%%f" "%WRAPPER_DIR%\src\bq\" >nul
    echo Copied %%~nxf
)

rem ===== Step 4: Build wheel =====
echo ===== Building wheel package =====
pushd "%INSTALL_DIR%"
py -m pip install --upgrade pip setuptools wheel
py -m pip wheel . -w "%INSTALL_DIR%"
popd

echo ===== Done =====
echo Wheel packages are in: %INSTALL_DIR%
dir "%INSTALL_DIR%\*.whl" 2>nul
