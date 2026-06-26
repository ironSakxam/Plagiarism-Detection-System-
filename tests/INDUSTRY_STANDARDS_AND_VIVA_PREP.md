# Industry Standards & Viva Preparation Guide

This document explains the industry-standard software engineering practices implemented in the Plagiarism Detection System and provides a comprehensive set of potential Viva Voce (oral examination) questions and answers.

---

## 1. Industry Standard Practices Applied & Why

### 1.1 Proper Scope and Namespace Isolation
* **Practice**: The entire project uses a custom namespace `pd` (Plagiarism Detector) and strictly avoids `using namespace std;` in header files.
* **Why**: Putting `using namespace std;` in header files causes "namespace pollution." Any file that `#include`s the header automatically imports all names from `std` into the global namespace, causing potential naming collisions (e.g., if you define a custom `count` function and `#include <algorithm>`). By using explicit scope resolution (`std::string`, `pd::Document`), the code is self-documenting, safer, and modular.

### 1.2 Object-Oriented Design (OOP) Principles
* **Encapsulation (Information Hiding)**:
  * Classes like `pd::UI`, `pd::Analyzer`, and `pd::Tokenizer` keep member variables (such as UI screen state or active filters) `private`.
  * External classes can only interact through defined, clean public APIs (e.g., `ui.render(...)`). This prevents external code from putting an object into an invalid internal state.
* **Abstraction**:
  * High-level orchestration in [main.cpp](file:///c:/Users/saksham/.vscode/MARK/Project-2/main.cpp) does not know or care how files are read from the disk or how Raylib renders text. It only calls high-level functions like `fileManager.readFile()` or `ui.render()`.
* **Composition ("HAS-A" Relationship)**:
  * The `Analyzer` class has a member of type `Tokenizer`. This is a classic composition pattern. The analyzer delegates the token extraction process to its internal `Tokenizer` instance rather than implementing parsing itself.
* **Separation of Concerns (MVC-lite)**:
  * **Model**: Structs like `Document` and `ComparisonItem` hold plain data.
  * **View**: `UI` class manages all graphics, widgets, and layout.
  * **Controller**: `main.cpp` coordinates directory scanning, user inputs, database reload loops, and passes data between the model and view.

### 1.3 Safe Resource Management & RAII (Resource Acquisition Is Initialization)
* **Practice**: The codebase relies entirely on Standard Library (STL) containers (`std::vector`, `std::string`, `std::set`) and standard objects.
* **Why**: There are no raw `new` and `delete` expressions in the core logic. By using modern C++ resource managers, memory is automatically freed when containers go out of scope, eliminating the risk of memory leaks (dangling pointers).

### 1.4 Modern C++ Standard (C++17) File Handling
* **Practice**: The project uses `<filesystem>` (`std::filesystem::path`, `std::filesystem::directory_iterator`, `std::filesystem::last_write_time`) for all path manipulations and directory scans.
* **Why**: Traditional C-style string manipulation (`char[]`) and platform-specific file directories (like Win32 API `FindFirstFile`) are error-prone and non-portable. C++17 `<filesystem>` is cross-platform, handles directories gracefully, and prevents buffer overflow vulnerabilities.

### 1.5 Defensive Programming & Exception Handling
* **Practice**: Accessing directory properties or reading files is wrapped in `try-catch` blocks (e.g., `getCurrentStates` in `main.cpp`).
* **Why**: If a file is locked temporarily by the operating system, a text editor, or a compiler, the program will not crash. Instead, it catches the exception and safely continues to the next frame.

### 1.6 Decoupled Performance Management
* **Practice**: Directory watching is rate-limited. The system checks for folder changes only once every 60 frames (~1 second) instead of every single frame.
* **Why**: Scanning a directory on the disk is a slow, blocking I/O operation. Running it every frame (60 times a second) would bottleneck the CPU and make the GUI lag. Rate-limiting it keeps the app responsive.

---

## 2. Viva Voce Questions & Answers

### Q1: What is the primary algorithm used to compute plagiarism, and how does it work?
**Answer**: 
The system uses the **Jaccard Similarity Coefficient**.
1. First, documents are preprocessed into sets of unique tokens (words/code segments).
2. The Jaccard score is calculated as the size of the intersection of both token sets divided by the size of their union:
   $$\text{Jaccard}(A, B) = \frac{|A \cap B|}{|A \cup B|}$$
3. The result is a value between `0.0` (completely different) and `1.0` (identical token sets).

---

### Q2: Why did you implement some structures as `struct` (like `Document` and `ComparisonItem`) and others as `class` (like `Analyzer` and `UI`)?
**Answer**:
* **`struct`**: Used for **Plain Old Data (POD)** structures. They have no behavior (methods) and only exist to group related variables together. All fields are `public` by default.
* **`class`**: Used when we need encapsulation (hiding state) and behavior. They contain private state variables and expose public methods to manipulate or query that state.

---

### Q3: What is "namespace pollution," and how does this project prevent it?
**Answer**:
Namespace pollution occurs when too many names are introduced into the global scope (e.g., using `using namespace std;` globally or in headers). This increases the chance of name collisions.
We prevent it by:
1. Wrapping all our project classes in the `pd` namespace.
2. Avoiding `using namespace std;` in header files, opting for explicit scoping like `std::vector` instead.

---

### Q4: Explain the hot-reloading (live folder monitoring) mechanism.
**Answer**:
In `main.cpp`, we store a cache of files along with their last modified timestamps (`std::filesystem::last_write_time`). Every 60 frames (~1 second):
1. We scan the directory again to get the current files and their write times.
2. We compare the current states to our cached states.
3. If they don't match (meaning a file was added, deleted, or edited), we trigger `reloadDatabase()` to parse the documents again and update the UI in real time.

---

### Q5: What is tokenization, and why is it necessary before comparing files?
**Answer**:
Tokenization is the process of breaking down a raw code file or text file into individual words or symbols (tokens), while discarding irrelevant characters.
It is necessary because comparing raw text directly is easily bypassed by adding spaces, tabs, comments, or changing letter casing. Our `Tokenizer`:
1. Strips all single-line (`//`) and multi-line (`/* */`) comments.
2. Converts all text to lowercase to make the check case-insensitive.
3. Normalizes literal strings to `"STR"` and characters to `"CHR"`.
This ensures we compare the actual structure of the code, not just formatting.

---

### Q6: What does the `&` and `const` mean in a parameter list like `double computeJaccard(const Document& a, const Document& b)`?
**Answer**:
* **`const`**: Guarantees that the function will not modify the documents being passed. This prevents accidental data corruption.
* **`&` (Reference)**: Passes the arguments by reference instead of copying them. Copying large vectors of tokens is slow and wastes memory; passing by reference is fast because it only passes a memory address.

---

### Q7: If you run `g++ main.cpp` from the command line, why does compilation fail? How does the build system resolve this?
**Answer**:
It fails because C++ compiles files individually. `main.cpp` only contains declarations of the classes (from the header files). The compiler needs the actual compiled implementation code (machine instructions) contained in `Analyzer.cpp`, `UI.cpp`, `Tokenizer.cpp`, etc.
The [build.bat](file:///c:/Users/saksham/.vscode/MARK/Project-2/build.bat) script compiles all source files together and links them:
```bash
g++ -std=c++17 main.cpp src/Analyzer.cpp src/Tokenizer.cpp src/FileManager.cpp src/UI.cpp src/Document.cpp src/FileDialog.cpp -o plagiarism.exe -I include -I raylib/src -L raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32
```
It also links external graphic libraries (`-lraylib`, `-lopengl32`) and native dialog handlers (`-lcomdlg32`).

---

### Q8: What would happen if a file read failed or was empty? How does the system handle this?
**Answer**:
Our `FileManager::readFile` safely returns an empty string `""` on failure. When loading the database in `main.cpp`, we check `if (!doc.rawText.empty())` before tokenizing and adding it to the active documents list. This prevents the program from performing division-by-zero or set calculations on empty inputs, making the program robust.
