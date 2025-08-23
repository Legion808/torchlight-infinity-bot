@echo off
echo Building Torchlight Infinity Farm Bot...

REM Check for Visual Studio Build Tools
set VS_PATH=""
for /f "tokens=*" %%i in ('dir "C:\Program Files*" /s /b 2^>nul ^| findstr "MSBuild.exe" ^| findstr "Current"') do (
    set VS_PATH=%%i
    goto :found_vs
)

:found_vs
if %VS_PATH%=="" (
    echo ERROR: Visual Studio Build Tools not found!
    echo Please install Visual Studio 2019 or newer, or Visual Studio Build Tools
    echo Download from: https://visualstudio.microsoft.com/downloads/
    goto :error
)

echo Found MSBuild at: %VS_PATH%

REM Install NuGet packages if needed
if not exist "packages" (
    echo Installing NuGet packages...
    if exist "C:\Program Files\Microsoft Visual Studio\*\*\Common7\IDE\CommonExtensions\Microsoft\NuGet\nuget.exe" (
        for /f "tokens=*" %%i in ('dir "C:\Program Files\Microsoft Visual Studio\*\*\Common7\IDE\CommonExtensions\Microsoft\NuGet\nuget.exe" /s /b 2^>nul') do (
            "%%i" restore ProcessMemoryReader.sln
            goto :build
        )
    )
    echo Warning: Could not find NuGet, you may need to install packages manually
)

:build
echo Building project...
"%VS_PATH%" ProcessMemoryReader.sln /p:Configuration=Release /p:Platform=x64 /v:minimal

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL!
    echo ========================================
    echo Executable location: ProcessMemoryReader\x64\Release\ProcessMemoryReader.exe
    echo.
    echo To run the bot:
    echo 1. Start Torchlight Infinity
    echo 2. Run ProcessMemoryReader.exe as Administrator
    echo 3. Follow the on-screen menu
    echo.
) else (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    echo Please check the error messages above.
    echo Common issues:
    echo - Missing Visual Studio C++ components
    echo - Missing Windows SDK
    echo - Missing nlohmann/json NuGet package
    echo.
    goto :error
)

goto :end

:error
echo.
echo Manual build instructions:
echo 1. Install Visual Studio 2019 or newer with C++ Desktop Development workload
echo 2. Open ProcessMemoryReader.sln in Visual Studio
echo 3. Right-click solution -> Restore NuGet Packages
echo 4. Build -> Build Solution (Ctrl+Shift+B)
echo 5. Set configuration to Release x64
echo.
exit /b 1

:end
pause
