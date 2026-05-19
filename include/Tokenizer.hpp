#pragma once
#include <string>
#include <vector>

namespace pd {

// The Tokenizer class is responsible for converting raw source code 
// into a clean list of words/symbols (tokens).
class Tokenizer {
public:
    // The only function the outside world needs to call
    std::vector<std::string> extractTokens(const std::string& rawCode);

private:
    // Internal helper functions (Encapsulation - hidden from outside)
    std::string stripComments(const std::string& code);
    std::string lowercaseText(const std::string& code);
};

} // namespace pd
