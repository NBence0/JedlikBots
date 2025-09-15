@echo off
setlocal enabledelayedexpansion

REM Forrás és cél mappák
set "src=%~dp0spi"
set "dst=%~dp0spi_json"

REM Cél mappa létrehozása
if not exist "%dst%" mkdir "%dst%"

REM Bejárjuk az összes fájlt rekurzívan
for /R "%src%" %%F in (*) do (
    REM relatív útvonal kiszámítása
    set "rel=%%~dpF"
    set "rel=!rel:%src%=!"

    REM cél mappa létrehozása
    if not exist "%dst%!rel!" mkdir "%dst%!rel!"

    REM fájlnév + .json kiterjesztés
    copy "%%F" "%dst%!rel!%%~nF.json" >nul
)

echo ✅ Kész! A fájlok átmásolva .json kiterjesztéssel a "%dst%" mappába.
pause
