@echo off
setlocal

echo Building plagiarism detection project...

set SRC=main.cpp src\Analyzer.cpp src\Tokenizer.cpp src\FileManager.cpp src\Document.cpp src\UI.cpp src\FileDialog.cpp
set FLAGS=-std=c++17 -Wall -Wextra -I include -I raylib\src -L raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32

set "LOCAL_GPP=%~dp0raylib\mingw64\bin\g++.exe"
if exist "%LOCAL_GPP%" (
    echo Executing: "%LOCAL_GPP%" %SRC% -o plagiarism.exe %FLAGS%
    "%LOCAL_GPP%" %SRC% -o plagiarism.exe %FLAGS%
) else (
    echo Executing: g++ %SRC% -o plagiarism.exe %FLAGS%
    g++ %SRC% -o plagiarism.exe %FLAGS%
)

if errorlevel 1 (
    echo Build failed.
    exit /b %errorlevel%
)

echo Build completed successfully.
exit /b 0
