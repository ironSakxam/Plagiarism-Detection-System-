# Plagiarism Detection System

A professional C++ plagiarism detection application built with Raylib.
This repository contains the scaffolding and architecture for a modular plagiarism detection system that can scan, process, and compare documents using a C++ backend and a Raylib UI.

## Project Summary

This project is a C++-based plagiarism detection system that will:

- scan directories for source documents
- read and normalize text files
- tokenize and process document content
- compute similarity using Jaccard similarity
- display results with a lightweight Raylib user interface

The system is designed as a modular architecture with separate components for file management, document storage, analysis, and rendering.

## Team

- Saksham Mandal
- Alex Adhikari
- Sumit Kr. Shah

## Tech Stack

- Language: C++ (C++17 standard)
- Graphics / UI: Raylib
- Compiler: `g++`
- Build system: batch script for Windows (`build.bat`)
- Platform: Windows
- Project layout: modular headers in `include/`, implementation files in `src/`

## Architecture

The planned architecture includes the following core modules:

- **File Manager**: scans folders, lists files, and performs file I/O
- **Document Module**: stores raw text and processed token data
- **Analyzer**: handles preprocessing, tokenization, and similarity computation
- **UI Module**: renders the application window and displays results with Raylib

### Similarity Algorithm

The first comparison method is:

- **Jaccard Similarity**

Formula:

```
J(A, B) = |A ∩ B| / |A ∪ B|
```

This will be used to measure overlap between token sets extracted from documents.

## Repository Structure

```
Project/
├── .vscode/           # VS Code launch and task configs
├── assets/            # optional icons, fonts, sample resources
├── data/              # example input documents and datasets
├── include/           # public module headers
│   ├── Analyzer.h
│   ├── Document.h
│   ├── FileManager.h
│   ├── UI.h
│   └── Utils.h
├── src/               # module implementation files
│   ├── Analyzer.cpp
│   ├── Document.cpp
│   ├── FileManager.cpp
│   ├── Tokenizer.cpp
│   ├── UI.cpp
│   └── Utils.cpp
├── tests/             # automated tests and validation code
├── raylib/            # local Raylib source and library files
├── build.bat          # build helper script
├── main.cpp           # application entry point
├── README.md          # project overview and setup
├── README2.md         # architecture and development plan
└── Roadmap.text       # project roadmap and algorithm notes
```

## Getting Started

### Prerequisites

- Windows
- `g++` with C++17 support
- Local Raylib source present in `raylib/src`

### Build

Run the helper script:

```powershell
build.bat
```

Or build manually:

```bash
g++ -std=c++17 main.cpp -o game.exe -I raylib/src -L raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Run

- Run `game.exe` from the project folder
- In VS Code, press `F5` to debug with the existing configuration

## Development Plan

The project will be developed in stages:

1. implement file scanning and document loading
2. build the document data model
3. add tokenization and text preprocessing
4. compute Jaccard similarity for document comparisons
5. design Raylib UI screens for file selection and similarity results
6. connect analysis results to the UI

## Notes

- The current repository is scaffolded for C++ development.
- Module implementations are intentionally separated so the system remains extensible.
- The UI should remain independent of core analysis logic.
- Future updates will add more advanced similarity metrics and better visualization.
