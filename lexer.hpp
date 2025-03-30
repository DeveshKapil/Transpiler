#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Lexer {
public:
    explicit Lexer(std::string source);
    std::unique_ptr<Token> nextToken();
    std::vector<std::unique_ptr<Token>> tokenize();
    
    bool hasError() const { return !error_message.empty(); }
    const std::string& getError() const { return error_message; }

private:
    static const std::unordered_map<std::string, TokenType> keywords;
    static const std::unordered_map<std::string, TokenType> operators;
    
    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;
    std::string error_message;
    
    bool isAtEnd() const { return current >= source.length(); }
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    
    static bool isDigit(char c) { return c >= '0' && c <= '9'; }
    static bool isAlpha(char c) { 
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }
    static bool isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }
    static bool isHexDigit(char c) {
        return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }
    
    std::unique_ptr<Token> scanToken();
    std::unique_ptr<Token> identifier();
    std::unique_ptr<Token> number();
    std::unique_ptr<Token> string();
    std::unique_ptr<Token> character();
    
    std::unique_ptr<Token> makeToken(TokenType type) const;
    std::unique_ptr<Token> makeToken(TokenType type, const std::string& text) const;
    std::unique_ptr<Token> errorToken(const std::string& message);
    
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    
    std::string currentLexeme() const {
        return source.substr(start, current - start);
    }
};
#endif