#include "Analyzer.hpp"
#include <algorithm>
#include <iterator>
#include <set>

namespace pd {

// Asks the Tokenizer to process the raw text, and saves it in the Document.
void Analyzer::preprocess(Document& doc) {
    doc.tokens = tokenizer.extractTokens(doc.rawText);
}

// Math logic: Jaccard = Intersection / Union
double Analyzer::
computeJaccard(const Document& a, const Document& b) {
    
    // Put tokens into sets to automatically remove duplicates
    std::set<std::string> setA(a.tokens.begin(), a.tokens.end());
    std::set<std::string> setB(b.tokens.begin(), b.tokens.end());

    if (setA.empty() && setB.empty()) {
        return 0.0;
    }

    std::set<std::string> intersection;
    std::set_intersection(
        setA.begin(), setA.end(),
        setB.begin(), setB.end(),
        std::inserter(intersection, intersection.begin())
    );

    std::set<std::string> unionSet;
    std::set_union( 
        setA.begin(), setA.end(),
        setB.begin(), setB.end(),
        std::inserter(unionSet, unionSet.begin())
    );

    return static_cast<double>(intersection.size()) /
           static_cast<double>(unionSet.size());
}

} // namespace pd
