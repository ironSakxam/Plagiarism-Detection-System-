# Code Explanation: Plagiarism Detection Analyzer

This document explains the logic behind the code added to the `src/` and `include/` folders so far. The system currently consists of three main modules: **Utils**, **Tokenizer**, and **Analyzer**.

## 1. Utils (`Utils.hpp` / `Utils.cpp`)

The `Utils` module is responsible for the very first step of the pipeline: cleaning up the raw text before we try to analyze it. When dealing with source code, we don't care about comments or the exact text inside strings.

### `stripComments()`
This function reads the source code character by character and removes things that aren't structural code:
- **`//` (Single-line comments):** If it sees `//`, it skips all characters until it hits a newline (`\n`).
- **`/* */` (Multi-line comments):** If it sees `/*`, it skips everything until it finds `*/`.
- **`"..."` (String literals):** It skips everything inside quotes, replacing it with the word `STR`. This means `print("Hello")` and `print("World")` look identical to the analyzer.
- **`'...'` (Char literals):** Similar to strings, replaced with `CHR`.

### `normalizeText()`
This function calls `stripComments()` first, and then converts every remaining letter to **lowercase**. This ensures that `MyVariable` and `myvariable` are treated as the exact same word later on.

---

## 2. Tokenizer (`Tokenizer.hpp` / `Tokenizer.cpp`)

Once the code is stripped of comments and lowercased, the Tokenizer breaks it down into bite-sized pieces called **tokens**.

### `splitIntoTokens()`
Unlike normal text where you just split by spaces, C++ code has symbols grouped together without spaces (e.g., `if(a==b){`). This scanner is smart enough to handle that:
- **Identifiers/Keywords:** If it sees a letter (like `i` in `int`), it grabs the whole word (`int`).
- **Numbers:** If it sees a number (like `5` or `3.14`), it doesn't care what the number is. It just replaces it with the token `NUM`. This stops someone from bypassing plagiarism detection just by changing `int x = 10;` to `int x = 20;`.
- **Symbols/Operators:** If it sees a symbol like `{`, `(`, `=`, or `+`, it treats each single symbol as its own token. This preserves the *structure* of the code.
- **Whitespace:** It completely ignores spaces, tabs, and newlines.

### `buildTokenSet()`
This takes the list of tokens from `splitIntoTokens()` and puts them into a `std::set`. In C++, a `set` automatically removes duplicates. So if the code uses the word `int` fifty times, the set only stores the word `int` once. This is required for the Jaccard algorithm.

---

## 3. Analyzer (`Analyzer.hpp` / `Analyzer.cpp`)

This is the brain of the operation. It ties the Utils and Tokenizer together to calculate the final plagiarism score.

### `Analyzer::preprocess()`
This simply takes a raw `Document`, passes its text through `normalizeText()` (to strip comments/lowercase), and then passes it through `splitIntoTokens()`. It stores the resulting list of tokens back inside the `Document` object so it's ready for comparison.

### `Analyzer::computeJaccard()`
This calculates the **Jaccard Similarity Index** between two preprocessed documents. 

The formula is: **Intersection ÷ Union**

1. **Intersection (`std::set_intersection`):** It finds all the unique tokens that appear in *both* Document A and Document B.
2. **Union (`std::set_union`):** It combines all the unique tokens from Document A and Document B, removing any overlaps so each token is only counted once.
3. **Calculation:** It divides the size of the Intersection by the size of the Union. 

**Result:** A number between `0.0` (completely different) and `1.0` (100% identical).

> [!NOTE]
> Because structural tokens like `{`, `}`, `(`, `)`, and `;` are shared in almost all C++ code, even completely different programs will usually have a base similarity score of around `0.25`. A score above `0.6` or `0.7` typically indicates strong structural similarities (plagiarism).
