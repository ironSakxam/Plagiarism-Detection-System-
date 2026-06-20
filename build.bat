@echo off
setlocal

set SRC=main.cpp src\Analyzer.cpp src\Tokenizer.cpp src\FileManager.cpp src\Document.cpp src\UI.cpp src\FileDialog.cpp
set FLAGS=-std=c++17 -I include -I raylib\src -L raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32

set "LOCAL_GPP=%~dp0raylib\mingw64\bin\g++.exe"
if exist "%LOCAL_GPP%" (
    "%LOCAL_GPP%" %SRC% -o plagiarism.exe %FLAGS%
) else (
    g++ %SRC% -o plagiarism.exe %FLAGS%
)
exit /b %errorlevel%
