# PowerShell build script for Windows (gcc/MinGW)
# Usage: Open PowerShell (or Git Bash) and run: .\build-windows.ps1
$ErrorActionPreference = 'Stop'

$srcDir = "enigma\src"
$buildDir = "enigma\build"
$binDir = "bin"
$out = Join-Path $buildDir "enigma.exe"

if (-not (Test-Path $buildDir)) { New-Item -ItemType Directory -Path $buildDir | Out-Null }

$cmd = "gcc -std=c11 -DWIN32 -O2 $srcDir\config.c $srcDir\decrypt.c $srcDir\encrypt.c $srcDir\key-parser.c $srcDir\main.c -I enigma\include -o $out"
Write-Host "Running: $cmd"
& cmd /c $cmd
Write-Host "Built $out"

if (-not (Test-Path $binDir)) { New-Item -ItemType Directory -Path $binDir | Out-Null }
Copy-Item -Path $out -Destination (Join-Path $binDir "enigma.exe") -Force
Write-Host "Copied to $binDir\enigma.exe"
Write-Host "Done."
