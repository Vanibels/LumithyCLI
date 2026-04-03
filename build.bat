@echo off
echo Building the project...
g++ lumithy.cpp -o out\lumithy
if %errorlevel% neq 0 (
    echo Build failed.
    pause
    exit /b %errorlevel%
) else (
    echo Build succeeded.
)
.\out\lumithy -o 
.\out\lumithy -o -h
.\out\lumithy -o -e
.\out\lumithy launch -h
pause