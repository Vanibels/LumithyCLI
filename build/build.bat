@echo off
setlocal enabledelayedexpansion

if not exist ..\out mkdir ..\out
g++ ..\src\lumithy.cpp -o ..\out\lumithy.exe -luser32 -ladvapi32 -lshlwapi -std=c++17

if %errorlevel% neq 0 (
    echo [ERROR] Build failed. Check your C++ code.
    pause
    exit /b %errorlevel%
)
echo [SUCCESS] Build succeeded.
..\out\lumithy.exe -o -h
pause
