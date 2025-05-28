#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stack>
#include "tokens.hpp"

class Lexer {
public:
    explicit Lexer(const std::string& source);

    // Tokenize the entire source code and return a vector of tokens
    std::vector<std::unique_ptr<Token>> tokenize();
    

private:
    const std::string& source_;   // Source code to tokenize
    size_t pos_ = 0;              // Current position in source_
    int line_ = 1;                // Current line number (starts at 1)
    int column_ = 1;              // Current column number (starts at 1)
    std::string errorMessage_;    // error message

    size_t tokenStartPos_ = 0;    // Start position of the current token

    // Macro definitions: macro name -> replacement text
    std::unordered_map<std::string, std::string> macros_;

    // Stack tracking conditional compilation states (#ifdef, #ifndef, #else, #endif)
    std::vector<bool> conditionalStack_;

    // If true, lexer is currently skipping tokens due to false condition in preprocessing
    bool skipping_ = false;

    // --- Helper methods for character reading ---
    char peek() const;            // Look at current character without advancing
    char peekNext() const;        // Look at next character without advancing
    char advance();               // Consume current character and advance position
    bool match(char expected);    // If current char matches expected, consume it and return true

    void skipWhitespaceAndComments();  // Skip spaces, tabs, newlines, and comments

    // --- Lexing different types of tokens ---
    std::unique_ptr<Token> lexToken();
    std::unique_ptr<Token> lexIdentifierOrKeyword();
    std::unique_ptr<Token> lexNumber();
    std::unique_ptr<Token> lexString();
    std::unique_ptr<Token> lexChar();
    std::unique_ptr<Token> lexPreprocessorDirective();

    // --- Macro handling helpers ---
    void defineMacro(const std::string& name, const std::string& value);
    bool isMacroDefined(const std::string& name) const;
    void updateSkipping();                // Update skipping_ state based on conditionalStack_

    std::string expandMacro(const std::string& text) const;  // Expand macro if defined

    // --- Keyword and identifier handling ---
    TokenType keywordLookup(const std::string& identifier) const;

    // --- Token creation helpers ---
    std::unique_ptr<Token> makeToken(TokenType type, const std::string& text);
    std::unique_ptr<Token> errorToken(const std::string& message);

    // --- Preprocessor Expression Evaluation Helpers ---
    void skipSpaces(const std::string& expr, size_t& idx);
    bool matchKeyword(const std::string& expr, size_t& idx, const std::string& keyword);
    int evalIfExpression(const std::string& expr);
    int preparseExpression(const std::string& expr, size_t& idx);
    int preparseLogicalOr(const std::string& expr, size_t& idx);
    int preparseLogicalAnd(const std::string& expr, size_t& idx);
    int preparseEquality(const std::string& expr, size_t& idx);
    int preparseRelational(const std::string& expr, size_t& idx);
    int preparseUnary(const std::string& expr, size_t& idx);
    int preparsePrimary(const std::string& expr, size_t& idx);

    // --- Utility ---
    bool isAtEnd() const;         // Check if reached end of source
};

#endif // LEXER_HPP
