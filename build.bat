@echo off
setlocal

set "LOCAL_GPP=%~dp0raylib\mingw64\bin\g++.exe"
if exist "%LOCAL_GPP%" (
    "%LOCAL_GPP%" main.cpp -o game.exe -I raylib\src -L raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm
) else (
    g++ main.cpp -o game.exe -I raylib\src -L raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm
)
exit /b %errorlevel%
