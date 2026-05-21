# Plagiarism Detection System - Architecture Overview

This is the second README for the C++ plagiarism detection project. It describes the intended folder architecture, system architecture, and how the project should be organized based on the roadmap.

---

## Project Vision

Build a C++ plagiarism detection application with:

- a file manager for reading and scanning source documents
- an analyzer for preprocessing, tokenization, and similarity computation
- a document model for storing raw and processed text
- a Raylib UI for presenting results visually
- a similarity engine using Jaccard similarity as the first algorithm


## High-Level System Architecture

```
User
  │
  ▼
UI Module (Raylib)
  │
  ▼
Controller / App Core
  │
  ├─ File Manager
  │    ├─ read directories
  │    ├─ load files
  │    └─ handle file I/O
  │
  ├─ Document Module
  │    ├─ store raw text
  │    ├─ store tokens and metadata
  │    └─ expose document data to the analyzer
  │
  └─ Analyzer
       ├─ preprocessing
       ├─ tokenization
       └─ similarity computation
             └─ Jaccard similarity
```

---

## Suggested Folder Layout

```
Project-2/
├── build.bat
├── main.cpp
├── README.md
├── README2.md
├── Roadmap.text
├── raylib/          # local Raylib source and library files
├── .vscode/         # VS Code launch and task configs
├── include/         # public headers for modules
│   ├── FileManager.h
│   ├── Document.h
│   ├── Analyzer.h
│   ├── UI.h
│   └── Utils.h
├── src/             # implementation files
│   ├── FileManager.cpp
│   ├── Document.cpp
│   ├── Analyzer.cpp
│   ├── UI.cpp
│   ├── Tokenizer.cpp
│   └── Utils.cpp
├── assets/          # optional resource files, icons, fonts, sample documents
├── tests/           # automated tests for parser, similarity, and file handling
└── data/            # example input files for testing and demo
```

> Note: `main.cpp` can stay at the root and act as the application entry point.

---

## Module Responsibilities

### 1. File Manager

Responsible for:

- scanning directories
- listing candidate files
- reading file contents into memory
- detecting supported file extensions

Expected components:

- `FileManager.h`
- `FileManager.cpp`

### 2. Analyzer

Responsible for:

- text preprocessing (lowercasing, removing punctuation, optional stop words)
- tokenization into words, n-grams, or other units
- computing similarity scores

Expected components:

- `Analyzer.h`
- `Analyzer.cpp`
- `Tokenizer.cpp`

### 3. Document Module

Responsible for:

- storing the original file path and raw text
- storing token sets or processed representations
- exposing document metadata to the analyzer and UI

Expected components:

- `Document.h`
- `Document.cpp`

### 4. UI Module

Responsible for:

- rendering the main window using Raylib
- showing file selection controls
- displaying similarity results and highlighted comparisons

Expected components:

- `UI.h`
- `UI.cpp`

---

## Algorithm: Jaccard Similarity

Use the Jaccard formula to compare two document token sets:

```
J(A, B) = |A ∩ B| / |A ∪ B|
```

That means:

- Create a set of tokens for each document
- Count matched tokens
- Divide by the total unique tokens across both documents
- Use the result as a similarity score between 0 and 1

---

## Next Steps

1. Create `include/` and `src/` directories.
2. Move implementation into module files.
3. Keep `main.cpp` as the launcher that initializes Raylib and coordinates the modules.
4. Add a simple file list and result view in the UI.
5. Implement Jaccard similarity inside the analyzer.

---

## Notes for development

- Keep the codebase C++17-compatible.
- Keep Raylib usage limited to the UI module.
- Keep file handling and analysis logic separate from rendering.
- Use the folder structure to organize future features cleanly.
