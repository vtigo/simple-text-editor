@echo off
setlocal

if not "%OS%"=="Windows_NT" (
    echo error: build.bat requires Windows
    exit /b 1
)

if "%GLFW_DIR%"=="" set "GLFW_DIR=C:\external\GLFW"

where cl >nul 2>nul
if errorlevel 1 (
    set "VCVARS="
    for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do set "VCVARS=%%i\VC\Auxiliary\Build\vcvars64.bat"
    if not defined VCVARS (
        echo error: MSVC not found
        exit /b 1
    )
    call "%VCVARS%" >nul || exit /b 1
)

if not exist build mkdir build

set "CFLAGS=/nologo /MD /Zi /W4 /I%GLFW_DIR%\include"
set "LFLAGS=/link /LIBPATH:%GLFW_DIR%\lib glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib"

set "ROOT=%CD:\=/%"
set "INC=%GLFW_DIR:\=/%"
> compile_commands.json (
    echo [
    echo   {
    echo     "directory": "%ROOT%",
    echo     "command": "cl /nologo /MD /Zi /W4 /I%INC%/include /c src/main.c /Fobuild/main.obj",
    echo     "file": "src/main.c"
    echo   }
    echo ]
)

cl %CFLAGS% /Fo:build\ /Fd:build\ /Fe:build\editor.exe src\main.c %LFLAGS% || exit /b 1

