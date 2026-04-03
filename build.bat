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
.\out\lumithy -d -o otf C:\Users\vanibels\Code Dev\Cpp\Lumithy CLI\out
pause