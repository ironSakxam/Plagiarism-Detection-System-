#pragma once

#include "Document.hpp"
#include <string>
#include <vector>

namespace pd {

class Analyzer {
public:
    std::vector<std::string> tokenize(const std::string& text);
    void preprocess(Document& doc);
    double computeJaccard(const Document& a, const Document& b);
};

} // namespace pd
