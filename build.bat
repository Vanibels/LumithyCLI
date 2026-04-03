@echo off
setlocal enabledelayedexpansion

echo ======================================================
echo [STEP 1] Building LumithyCLI Project...
echo ======================================================
if not exist out mkdir out
g++ lumithy.cpp -o out\lumithy.exe -luser32 -ladvapi32 -lshlwapi -std=c++17

if %errorlevel% neq 0 (
    echo [ERROR] Build failed. Check your C++ code.
    pause
    exit /b %errorlevel%
)
echo [SUCCESS] Build succeeded.

echo.
echo ======================================================
echo [STEP 2] Testing Versioning and Config Creation
echo ======================================================
:: On lance sans argument pour générer le config.ini par défaut
.\out\lumithy.exe
echo.

echo ======================================================
echo [STEP 3] Testing Alias Management (ADD)
echo ======================================================
echo Adding 'docs' to [open] section...
.\out\lumithy.exe -a -o docs %USERPROFILE%\Documents
echo.
echo Adding 'program' with spaces to [launch] section...
.\out\lumithy.exe -a -l notepad C:\Windows\System32\notepad.exe
echo.

echo ======================================================
echo [STEP 4] Testing Navigation and Launch (OPEN/LAUNCH)
echo ======================================================
echo Testing 'open -e' (User Explorer)...
.\out\lumithy.exe -o -e
echo.
echo Testing 'open -h' (Section help)...
.\out\lumithy.exe -o -h
echo.
echo Testing 'launch -h' (Section help)...
.\out\lumithy.exe -l -h
echo.

echo ======================================================
echo [STEP 5] Testing Configuration (RELOAD/CONFIG)
echo ======================================================
echo Reloading config...
.\out\lumithy.exe -r
echo.

echo ======================================================
echo [STEP 6] Testing Removal (DELETE)
echo ======================================================
echo Deleting 'docs' alias...
.\out\lumithy.exe -d -o docs
echo.

echo ======================================================
echo [STEP 7] Final Help Check
echo ======================================================
.\out\lumithy.exe -h

echo.
echo [DONE] Battery test completed. Check the console logs.
pause