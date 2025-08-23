# PowerShell Build Script for Torchlight Infinity Farm Bot
Write-Host "Building Torchlight Infinity Farm Bot..." -ForegroundColor Green

# Find Visual Studio installation
$vsPath = ""
$msbuildPath = ""

# Look for VS 2019 or newer
$vsPaths = @(
    "C:\Program Files\Microsoft Visual Studio\2022\*\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2019\*\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\*\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files (x86)\Microsoft Visual Studio\2022\*\MSBuild\Current\Bin\MSBuild.exe"
)

foreach ($path in $vsPaths) {
    $found = Get-ChildItem $path -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($found) {
        $msbuildPath = $found.FullName
        Write-Host "Found MSBuild at: $msbuildPath" -ForegroundColor Yellow
        break
    }
}

if (-not $msbuildPath) {
    Write-Host "ERROR: Visual Studio Build Tools not found!" -ForegroundColor Red
    Write-Host "Please install Visual Studio 2019 or newer with C++ Desktop Development workload" -ForegroundColor Red
    Write-Host "Download from: https://visualstudio.microsoft.com/downloads/" -ForegroundColor Yellow
    exit 1
}

# Check for NuGet packages
if (-not (Test-Path "packages")) {
    Write-Host "Installing NuGet packages..." -ForegroundColor Yellow
    $nugetPath = Get-ChildItem "C:\Program Files*" -Recurse -Name "nuget.exe" -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($nugetPath) {
        & $nugetPath restore ProcessMemoryReader.sln
    } else {
        Write-Host "Warning: NuGet not found. You may need to install packages manually in Visual Studio." -ForegroundColor Yellow
    }
}

# Build the project
Write-Host "Building project..." -ForegroundColor Yellow
& $msbuildPath "ProcessMemoryReader.sln" /p:Configuration=Release /p:Platform=x64 /v:minimal

if ($LASTEXITCODE -eq 0) {
    Write-Host "" 
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "BUILD SUCCESSFUL!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "Executable location: ProcessMemoryReader\x64\Release\ProcessMemoryReader.exe" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "To run the bot:" -ForegroundColor Cyan
    Write-Host "1. Start Torchlight Infinity" -ForegroundColor White
    Write-Host "2. Run ProcessMemoryReader.exe as Administrator" -ForegroundColor White
    Write-Host "3. Follow the on-screen menu" -ForegroundColor White
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "BUILD FAILED!" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "Please check the error messages above." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Common issues:" -ForegroundColor Yellow
    Write-Host "- Missing Visual Studio C++ components" -ForegroundColor White
    Write-Host "- Missing Windows SDK" -ForegroundColor White
    Write-Host "- Missing nlohmann/json NuGet package" -ForegroundColor White
    Write-Host ""
    Write-Host "Manual build instructions:" -ForegroundColor Cyan
    Write-Host "1. Install Visual Studio 2019 or newer with C++ Desktop Development workload" -ForegroundColor White
    Write-Host "2. Open ProcessMemoryReader.sln in Visual Studio" -ForegroundColor White
    Write-Host "3. Right-click solution -> Restore NuGet Packages" -ForegroundColor White
    Write-Host "4. Build -> Build Solution (Ctrl+Shift+B)" -ForegroundColor White
    Write-Host "5. Set configuration to Release x64" -ForegroundColor White
    exit 1
}

Read-Host "Press Enter to continue"
