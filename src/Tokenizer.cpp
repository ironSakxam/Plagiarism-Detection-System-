#include "Tokenizer.hpp"
#include <cctype>

namespace pd {

// PUBLIC METHOD
std::vector<std::string> Tokenizer::extractTokens(const std::string& rawCode) {
    std::vector<std::string> tokens;
    
    // Step 1 & 2: Clean the text using our private helpers
    std::string noComments = stripComments(rawCode);
    std::string cleanCode = lowercaseText(noComments);
    
    // Step 3: Scan and extract tokens
    size_t i = 0;
    while (i < cleanCode.size()) {
        char c = cleanCode[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            i++;
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t start = i;
            while (i < cleanCode.size() &&
                   (std::isalnum(static_cast<unsigned char>(cleanCode[i])) || cleanCode[i] == '_')) {
                i++;
            }
            tokens.push_back(cleanCode.substr(start, i - start));
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            while (i < cleanCode.size() &&
                   (std::isdigit(static_cast<unsigned char>(cleanCode[i])) ||
                    cleanCode[i] == '.' || cleanCode[i] == 'x' ||
                    std::isalpha(static_cast<unsigned char>(cleanCode[i])))) {
                i++;
            }
            tokens.push_back("NUM");
            continue;
        }

        tokens.push_back(std::string(1, c));
        i++;
    }

    return tokens;
}

// PRIVATE HELPER 1: Remove Comments and Strings
std::string Tokenizer::stripComments(const std::string& code) {
    std::string result;
    result.reserve(code.size());
    size_t i = 0;
    
    while (i < code.size()) {
        if (i + 1 < code.size() && code[i] == '/' && code[i + 1] == '/') {
            while (i < code.size() && code[i] != '\n') i++;
            result += ' ';
        } else if (i + 1 < code.size() && code[i] == '/' && code[i + 1] == '*') {
            i += 2;
            while (i + 1 < code.size() && !(code[i] == '*' && code[i + 1] == '/')) i++;
            if (i + 1 < code.size()) i += 2;
            result += ' ';
        } else if (code[i] == '"') {
            i++;
            while (i < code.size() && code[i] != '"') {
                if (code[i] == '\\' && i + 1 < code.size()) i++;
                i++;
            }
            if (i < code.size()) i++;
            result += " STR ";
        } else if (code[i] == '\'') {
            i++;
            while (i < code.size() && code[i] != '\'') {
                if (code[i] == '\\' && i + 1 < code.size()) i++;
                i++;
            }
            if (i < code.size()) i++;
            result += " CHR ";
        } else {
            result += code[i];
            i++;
        }
    }
    return result;
}

// PRIVATE HELPER 2: Lowercase all text
std::string Tokenizer::lowercaseText(const std::string& code) {
    std::string result = code;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

} // namespace pd
