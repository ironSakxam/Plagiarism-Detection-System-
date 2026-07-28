#pragma once
#include "Document.hpp"
#include "Tokenizer.hpp"

namespace pd {

class Analyzer {
private:
    Tokenizer tokenizer;

public:
    void preprocess(Document& doc);
    double computeJaccard(const Document& a, const Document& b);
};

} // namespace pd
