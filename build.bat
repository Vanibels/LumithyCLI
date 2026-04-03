@echo off
echo Building the project...
g++ lumithy.cpp -o out\lumithy
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
) else (
    echo Build succeeded.
)
.\out\lumithy
pause