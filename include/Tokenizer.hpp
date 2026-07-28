#pragma once
#include <string>
#include <vector>

namespace pd {

class Tokenizer {
public:
    std::vector<std::string> extractTokens(const std::string& rawCode);

private:
    std::string stripComments(const std::string& code);
    std::string lowercaseText(const std::string& code);
};

} // namespace pd
