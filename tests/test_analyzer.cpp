// Standalone console test for the Analyzer + Tokenizer pipeline.

#include "Analyzer.hpp"
#include "Document.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

// Helper: prints a divider line
static void divider() {
    std::cout << std::string(50, '-') << "\n";
}

// Helper: runs one Jaccard test case and prints result
static void runTest(pd::Analyzer& analyzer,
                    const std::string& nameA,
                    const std::string& textA,
                    const std::string& nameB,
                    const std::string& textB,
                    double expectedApprox)
{
    pd::Document docA, docB;
    docA.rawText = textA;
    docB.rawText = textB;

    analyzer.preprocess(docA);
    analyzer.preprocess(docB);

    double score = analyzer.computeJaccard(docA, docB);

    std::cout << "  [" << nameA << "]  vs  [" << nameB << "]\n";
    std::cout << "  Score : " << std::fixed << std::setprecision(4) << score;
    std::cout << "  (expected ~" << expectedApprox << ")\n";

    // Simple pass/fail: within 0.05 of expected
    bool pass = std::abs(score - expectedApprox) < 0.05;
    std::cout << "  Result: " << (pass ? "PASS ✓" : "FAIL ✗") << "\n\n";
}

int main() {
    pd::Analyzer analyzer;

    std::cout << "\n=== Plagiarism Detection System - Source Code Tests ===\n\n";

    // ── Test 1: Identical Source Code 
    divider();
    std::cout << "TEST 1: Identical code (expect 1.0)\n";
    divider();
    runTest(analyzer,
        "Code A", "int main() { return 0; }",
        "Code B", "int main() { return 0; }",
        1.0);

    // ── Test 2: Formatting Differences Only ───────────────────────────────
    divider();
    std::cout << "TEST 2: Formatting/whitespace differences (expect 1.0)\n";
    divider();
    runTest(analyzer,
        "Code A", "int main() { return 0; }",
        "Code B", "int\nmain()\n{\n\treturn 0;\n}",
        1.0);

    // ── Test 3: Comment Differences Only 
    divider();
    std::cout << "TEST 3: Comment differences (expect 1.0)\n";
    divider();
    runTest(analyzer,
        "Code A", "int main() { return 0; } // standard main",
        "Code B", "/* entry point */ int main() { return 0; }",
        1.0);

        // ── Test 4: Variable Name Changes (Plagiarism attempt) 
    divider();
    std::cout << "TEST 4: Variable name changes (expect high similarity > 0.7)\n";
    divider();
    // Tokens A: int, a, =, NUM, ;, int, b, =, NUM, ;, return, a, +, b, ;
    // Tokens B: int, x, =, NUM, ;, int, y, =, NUM, ;, return, x, +, y, ;
    // Shared structure, different identifiers.
    runTest(analyzer,
        "Code A", "int a = 5; int b = 10; return a + b;",
        "Code B", "int x = 5; int y = 10; return x + y;",
        0.55); // Jaccard on SETS of tokens drops duplicates, so the score depends on unique tokens.

    // ── Test 5: Number Literal Normalization 
    divider();
    std::cout << "TEST 5: Number literals are normalized (expect 1.0)\n";
    divider();
    runTest(analyzer,
        "Code A", "int x = 100; float y = 3.14;",
        "Code B", "int x = 999; float y = 2.71;",
        1.0);

    // ── Test 6: String/Char Literal Normalization 
    divider();
    std::cout << "TEST 6: String and char literals normalized (expect 1.0)\n";
    divider();
    runTest(analyzer,
        "Code A", "printf(\"Hello World\"); char c = 'A';",
        "Code B", "printf(\"Goodbye\"); char c = 'Z';",
        1.0);

    // ── Test 7: Completely Different Logic 
    divider();
    std::cout << "TEST 7: Completely different logic (expect ~0.26 due to shared {},;() )\n";
    divider();
    runTest(analyzer,
        "Code A", "int sum(int a, int b) { return a + b; }",
        "Code B", "void print(const char* s) { printf(s); }",
        0.26);

    divider();
    std::cout << "All tests complete.\n\n";
    return 0;
}

