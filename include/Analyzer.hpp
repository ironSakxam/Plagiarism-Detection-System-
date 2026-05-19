#pragma once
#include "Document.hpp"
#include "Tokenizer.hpp"

namespace pd {

// The Analyzer class is responsible for comparing two documents.
class Analyzer {
private:
    // Composition: Analyzer HAS-A Tokenizer to help it do its job.
    Tokenizer tokenizer;

public:
    // Processes a document (extracts its tokens) so it is ready for comparison
    void preprocess(Document& doc);

    // Compares two processed documents and returns a score from 0.0 to 1.0
    double computeJaccard(const Document& a, const Document& b);
};

} // namespace pd
