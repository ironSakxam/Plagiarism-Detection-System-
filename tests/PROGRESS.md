# PROGRESS.md — Developer Reference

> **Purpose**: Documents what has been implemented so far, the API contracts, and integration notes for future GUI / FileManager work. Keep this updated as modules are completed.

---

## ✅ Completed Today (Session 1)

### Modules Implemented

| File | Status | Description |
|---|---|---|
| `include/Tokenizer.hpp` | ✅ Created | Declares `splitIntoTokens()` and `buildTokenSet()` |
| `src/Utils.cpp` | ✅ Implemented | `normalizeText()` — lowercase + strip non-alpha |
| `src/Tokenizer.cpp` | ✅ Implemented | `splitIntoTokens()`, `buildTokenSet()`, `Analyzer::tokenize()` |
| `src/Analyzer.cpp` | ✅ Implemented | `Analyzer::preprocess()`, `Analyzer::computeJaccard()` |
| `build.bat` | ✅ Fixed | Now compiles all `src/*.cpp` files with `-I include` |
| `tests/test_analyzer.cpp` | ✅ Created | 7 standalone console tests, no Raylib needed |

---

## ❌ Not Yet Implemented (Stubs)

| File | What's missing |
|---|---|
| `src/FileManager.cpp` | `scanDirectory()` and `readFile()` — needs `<filesystem>` iteration |
| `src/UI.cpp` | `initialize()`, `render()`, `shutdown()` — needs Raylib integration |
| `src/Document.cpp` | No methods needed (struct only) — can stay as stub |
| `main.cpp` | Still shows placeholder; needs to wire all modules together |
| `data/` | No sample `.txt` files for live testing |

---

## API Reference — Implemented Modules

### `pd::stripComments()` — `Utils.hpp`
```cpp
std::string stripComments(const std::string& code);
```
- Removes `//` and `/* */` C/C++ style comments
- Replaces string literals (`"..."`) with ` STR `
- Replaces char literals (`'...'`) with ` CHR `
- Essential first pass for source code so comments don't affect similarity

---

### `pd::normalizeText()` — `Utils.hpp`
```cpp
std::string normalizeText(const std::string& code);
```
- Calls `stripComments()` first
- Converts all remaining characters to **lowercase**
- Leaves operators and punctuation intact (vital for source code structure)

---

### `pd::splitIntoTokens()` — `Tokenizer.hpp`
```cpp
std::vector<std::string> splitIntoTokens(const std::string& code);
```
- **Code-aware scanner**:
  - Extracts keywords/identifiers as full words (`int`, `main`, `myVar`)
  - Normalizes numbers into a single `NUM` token (so `5` and `999` match)
  - Treats operators/symbols (`{`, `}`, `=`, `+`) as individual structural tokens
  - Completely ignores whitespace
- Input should already be normalized

---

### `pd::buildTokenSet()` — `Tokenizer.hpp`
```cpp
std::set<std::string> buildTokenSet(const std::string& text);
```
- Returns a **unique set** of tokens from normalized text
- Duplicates are automatically eliminated
- Used internally by `computeJaccard()`
- Example: `"the cat the mat"` → `{"cat", "mat", "the"}`

---

### `pd::Analyzer::tokenize()` — `Analyzer.hpp`
```cpp
std::vector<std::string> tokenize(const std::string& text);
```
- Combines `normalizeText()` + `splitIntoTokens()` in one call
- Returns ordered token list (with duplicates)

---

### `pd::Analyzer::preprocess()` — `Analyzer.hpp`
```cpp
void preprocess(Document& doc);
```
- Normalizes and tokenizes `doc.rawText`
- Stores result into `doc.tokens` **in-place**
- **Must be called before `computeJaccard()`**
- Modifies the document — pass by reference intentionally

---

### `pd::Analyzer::computeJaccard()` — `Analyzer.hpp`
```cpp
double computeJaccard(const Document& a, const Document& b);
```
- Computes **Jaccard Similarity Index** between two documents
- Formula: `J(A, B) = |A ∩ B| / |A ∪ B|`
- Returns value in `[0.0, 1.0]`
  - `0.0` = no shared tokens (completely different)
  - `1.0` = identical token sets (same content)
- **Requires `preprocess()` to be called on both documents first**
- Returns `0.0` safely if both documents are empty

---

## Correct Pipeline Order

```cpp
pd::Analyzer analyzer;

// 1. Create documents with raw text
pd::Document docA, docB;
docA.rawText = "some text here";
docB.rawText = "some other text";

// 2. Preprocess both (normalize + tokenize)
analyzer.preprocess(docA);
analyzer.preprocess(docB);

// 3. Compute similarity
double score = analyzer.computeJaccard(docA, docB);
// score is in [0.0, 1.0]
```

---

## Integration Notes for GUI (future `UI.cpp` + `main.cpp`)

> Read this before wiring the UI. Don't break what's already working.

1. **`UI.cpp` must NOT call `Analyzer`, `Tokenizer`, or `Utils` directly.**
   All analysis logic stays in `src/Analyzer.cpp`. The UI just receives results.

2. **`Document` is a plain struct** — no constructor, no destructor.
   Load `rawText` from `FileManager::readFile()`, then call `Analyzer::preprocess()`.

3. **Call `preprocess()` before every `computeJaccard()` call.**
   If you skip preprocessing, `doc.tokens` will be empty and Jaccard returns `0.0`.

4. **Jaccard score is not a percentage** — it's a ratio `[0.0, 1.0]`.
   Multiply by `100` to display as `%` in the UI.

5. **`computeJaccard()` takes `const Document&`** — it does NOT modify documents.
   Safe to call repeatedly on the same docs without re-preprocessing.

6. **Thread safety**: None of these modules use global state.
   Safe to use across multiple comparisons sequentially.

---

## Build Commands

### Full project (with Raylib GUI)
```bat
build.bat
```
Or manually:
```bash
g++ -std=c++17 main.cpp src/Analyzer.cpp src/Tokenizer.cpp src/Utils.cpp src/FileManager.cpp src/Document.cpp src/UI.cpp -o game.exe -I include -I raylib/src -L raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Test only (no Raylib needed)
```bash
g++ -std=c++17 tests/test_analyzer.cpp src/Analyzer.cpp src/Tokenizer.cpp src/Utils.cpp -I include -o tests/test_analyzer.exe
.\tests\test_analyzer.exe
```

---

## Test Results (Session 1 — Source Code Aware)

| Test | Scenario | Expected | Status |
|---|---|---|---|
| 1 | Identical code | 1.0 | ✅ PASS |
| 2 | Formatting/whitespace differences | 1.0 | ✅ PASS |
| 3 | Comment differences | 1.0 | ✅ PASS |
| 4 | Variable name changes (plagiarism) | ~0.55 | ✅ PASS |
| 5 | Number literal normalization | 1.0 | ✅ PASS |
| 6 | String/char literal normalization | 1.0 | ✅ PASS |
| 7 | Completely different logic | ~0.26* | ✅ PASS |

*\* Note: Even completely different C++ functions share structural tokens like `{`, `}`, `(`, `)`, `;` — so a base score of ~0.25 is normal for unrelated code. High similarity is usually > 0.6.*
