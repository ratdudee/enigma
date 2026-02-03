@echo off
REM Simple Windows build script for use with gcc (MinGW/MSYS or similar)
REM This compiles the enigma project and defines WIN32 macro for conditional code.


nset SRC_DIR=enigma\src
nset BUILD_DIR=enigma\build
nset BIN_DIR=bin
nset OUT=%BUILD_DIR%\enigma.exe
n
nif not exist %BUILD_DIR% mkdir %BUILD_DIR%
n
nREM compile all .c sources explicitly so MSYS/MinGW can find headers relatively
ngcc -std=c11 -DWIN32 -O2 %SRC_DIR%\config.c %SRC_DIR%\decrypt.c %SRC_DIR%\encrypt.c %SRC_DIR%\key-parser.c %SRC_DIR%\main.c -I enigma\include -o %OUT%
nif errorlevel 1 (
n    echo Compilation failed with error %errorlevel%.
n    exit /b %errorlevel%
n)
necho Built %OUT%
n
nif not exist %BIN_DIR% mkdir %BIN_DIR%
copy /Y %OUT% %BIN_DIR%\enigma.exe >nul
necho Copied to %BIN_DIR%\enigma.exe
necho Done.