@echo off
setlocal

set SRC=main.cpp src\Analyzer.cpp src\Tokenizer.cpp src\FileManager.cpp src\Document.cpp src\UI.cpp
set FLAGS=-std=c++17 -I include -I raylib\src -L raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm

set "LOCAL_GPP=%~dp0raylib\mingw64\bin\g++.exe"
if exist "%LOCAL_GPP%" (
    "%LOCAL_GPP%" %SRC% -o game.exe %FLAGS%
) else (
    g++ %SRC% -o game.exe %FLAGS%
)
exit /b %errorlevel%
