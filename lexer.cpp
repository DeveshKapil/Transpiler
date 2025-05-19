#include "lexer.hpp"
#include <cctype>
#include <iostream>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"int", TokenType::INT}, {"void", TokenType::VOID}, {"char", TokenType::CHAR},
    {"float", TokenType::FLOAT_TYPE}, {"double", TokenType::DOUBLE}, {"bool", TokenType::BOOL},
    {"class", TokenType::CLASS}, {"struct", TokenType::STRUCT}, {"enum", TokenType::ENUM},
    {"union", TokenType::UNION}, {"const", TokenType::CONST}, {"unsigned", TokenType::UNSIGNED},
    {"signed", TokenType::SIGNED}, {"short", TokenType::SHORT}, {"long", TokenType::LONG},
    {"static", TokenType::STATIC}, {"extern", TokenType::EXTERN}, {"register", TokenType::REGISTER},
    {"inline", TokenType::INLINE}, {"virtual", TokenType::VIRTUAL}, {"explicit", TokenType::EXPLICIT},
    {"friend", TokenType::FRIEND}, {"private", TokenType::PRIVATE}, {"public", TokenType::PUBLIC},
    {"protected", TokenType::PROTECTED}, {"if", TokenType::IF}, {"else", TokenType::ELSE},
    {"for", TokenType::FOR}, {"while", TokenType::WHILE}, {"do", TokenType::DO},
    {"switch", TokenType::SWITCH}, {"case", TokenType::CASE}, {"default", TokenType::DEFAULT},
    {"break", TokenType::BREAK}, {"continue", TokenType::CONTINUE}, {"return", TokenType::RETURN},
    {"goto", TokenType::GOTO}
};

const std::unordered_map<std::string, TokenType> Lexer::operators = {
    {"+", TokenType::PLUS}, {"-", TokenType::MINUS}, {"*", TokenType::STAR},
    {"/", TokenType::SLASH}, {"%", TokenType::PERCENT}, {"&", TokenType::AMPERSAND},
    {"|", TokenType::PIPE}, {"^", TokenType::CARET}, {"~", TokenType::TILDE},
    {"!", TokenType::EXCLAIM}, {"=", TokenType::EQUAL}, {"<", TokenType::LESS},
    {">", TokenType::GREATER}, {"+=", TokenType::PLUS_EQUAL}, {"-=", TokenType::MINUS_EQUAL},
    {"*=", TokenType::STAR_EQUAL}, {"/=", TokenType::SLASH_EQUAL}, {"%=", TokenType::PERCENT_EQUAL},
    {"&=", TokenType::AND_EQUAL}, {"|=", TokenType::OR_EQUAL}, {"^=", TokenType::XOR_EQUAL},
    {"<<", TokenType::LESS_LESS}, {">>", TokenType::GREATER_GREATER}, {"==", TokenType::EQUAL_EQUAL},
    {"!=", TokenType::NOT_EQUAL}, {"<=", TokenType::LESS_EQUAL}, {">=", TokenType::GREATER_EQUAL},
    {"&&", TokenType::AND_AND}, {"||", TokenType::OR_OR}, {"++", TokenType::PLUS_PLUS},
    {"--", TokenType::MINUS_MINUS}, {"->", TokenType::ARROW}, {"(", TokenType::LEFT_PAREN},
    {")", TokenType::RIGHT_PAREN}, {"{", TokenType::LEFT_BRACE}, {"}", TokenType::RIGHT_BRACE},
    {"[", TokenType::LEFT_BRACKET}, {"]", TokenType::RIGHT_BRACKET}, {";", TokenType::SEMICOLON},
    {",", TokenType::COMMA}, {".", TokenType::DOT}, {":", TokenType::COLON}, {"::", TokenType::SCOPE},
    {"#", TokenType::HASH}
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::unique_ptr<Token> Lexer::nextToken() {
    std::cout << "nextToken() at current=" << current << "\n" << std::flush;
    skipWhitespace();
    start = current;
    int startColumn = column;
    if (isAtEnd()) {
        std::cout << "Reached EOF\n" << std::flush;
        return std::unique_ptr<Token>(new Token(TokenType::END_OF_FILE, "", line, startColumn));
    }
    auto token = scanToken();
    std::cout << "nextToken() returning: " << token->toString() << "\n" << std::flush;
    return token;
}

std::vector<std::unique_ptr<Token>> Lexer::tokenize() {
    std::cout << "Starting tokenize()\n" << std::flush;
    std::vector<std::unique_ptr<Token>> tokens;
    while (true) {
        auto token = nextToken();
        if (token->type() == TokenType::END_OF_FILE) {
            std::cout << "Found EOF\n" << std::flush;
            tokens.push_back(std::move(token));
            break;
        }
        tokens.push_back(std::move(token));
    }
    
    std::cout << "tokenize() completed with " << tokens.size() << " tokens\n" << std::flush;
    return tokens;
}

char Lexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    column++;
    return true;
}

std::unique_ptr<Token> Lexer::scanToken() {
    char c = advance();
    int startColumn = column - 1;
    std::cout << "scanToken() char='" << c << "' at line=" << line << ", column=" << startColumn << "\n" << std::flush;
    
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();
    
    switch (c) {   
        case ')': return std::unique_ptr<Token>(new Token(TokenType::RIGHT_PAREN, ")", line, startColumn));
        case '{': return std::unique_ptr<Token>(new Token(TokenType::LEFT_BRACE, "{", line, startColumn));
        case '}': return std::unique_ptr<Token>(new Token(TokenType::RIGHT_BRACE, "}", line, startColumn));
         case '(': return std::unique_ptr<Token>(new Token(TokenType::LEFT_PAREN, "(", line, startColumn));
        case '[': return std::unique_ptr<Token>(new Token(TokenType::LEFT_BRACKET, "[", line, startColumn));
        case ']': return std::unique_ptr<Token>(new Token(TokenType::RIGHT_BRACKET, "]", line, startColumn));
        case ';': return std::unique_ptr<Token>(new Token(TokenType::SEMICOLON, ";", line, startColumn));
        case ',': return std::unique_ptr<Token>(new Token(TokenType::COMMA, ",", line, startColumn));
        case '.': return std::unique_ptr<Token>(new Token(TokenType::DOT, ".", line, startColumn));
        case '+':
            if (match('+')) return std::unique_ptr<Token>(new Token(TokenType::PLUS_PLUS, "++", line, startColumn));
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::PLUS_EQUAL, "+=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::PLUS, "+", line, startColumn));
        case '-':
            if (match('-')) return std::unique_ptr<Token>(new Token(TokenType::MINUS_MINUS, "--", line, startColumn));
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::MINUS_EQUAL, "-=", line, startColumn));
            if (match('>')) return std::unique_ptr<Token>(new Token(TokenType::ARROW, "->", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::MINUS, "-", line, startColumn));
        case '*':
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::STAR_EQUAL, "*=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::STAR, "*", line, startColumn));
        case '/':
            if (match('/')) { skipLineComment(); return nextToken(); }
            if (match('*')) { skipBlockComment(); return nextToken(); }
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::SLASH_EQUAL, "/=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::SLASH, "/", line, startColumn));
        case '=':
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::EQUAL_EQUAL, "==", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::EQUAL, "=", line, startColumn));
        case '!':
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::NOT_EQUAL, "!=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::EXCLAIM, "!", line, startColumn));
        case '<':
            if (match('<')) return std::unique_ptr<Token>(new Token(TokenType::LESS_LESS, "<<", line, startColumn));
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::LESS_EQUAL, "<=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::LESS, "<", line, startColumn));
        case '>':
            if (match('>')) return std::unique_ptr<Token>(new Token(TokenType::GREATER_GREATER, ">>", line, startColumn));
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::GREATER_EQUAL, ">=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::GREATER, ">", line, startColumn));
        case '&':
            if (match('&')) return std::unique_ptr<Token>(new Token(TokenType::AND_AND, "&&", line, startColumn));
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::AND_EQUAL, "&=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::AMPERSAND, "&", line, startColumn));
        case '|':
            if (match('|')) return std::unique_ptr<Token>(new Token(TokenType::OR_OR, "||", line, startColumn));
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::OR_EQUAL, "|=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::PIPE, "|", line, startColumn));
        case '^':
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::XOR_EQUAL, "^=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::CARET, "^", line, startColumn));
        case '%':
            if (match('=')) return std::unique_ptr<Token>(new Token(TokenType::PERCENT_EQUAL, "%=", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::PERCENT, "%", line, startColumn));
        case ':':
            if (match(':')) return std::unique_ptr<Token>(new Token(TokenType::SCOPE, "::", line, startColumn));
            return std::unique_ptr<Token>(new Token(TokenType::COLON, ":", line, startColumn));
        case '"': return string();
        case '\'': return character();
    }
    return errorToken(std::string("Unexpected character: ") + c);
}

std::unique_ptr<Token> Lexer::identifier() {
    std::cout << "identifier() at current=" << current << "\n" << std::flush;
    while (isAlphaNumeric(peek())) advance();
    std::string text = currentLexeme();
    std::cout << "Identifier text: '" << text << "'\n" << std::flush;
    auto it = keywords.find(text);
    if (it != keywords.end()) return std::unique_ptr<Token>(new Token(it->second, text, line, column - text.length()));
    return std::unique_ptr<Token>(new Token(TokenType::IDENTIFIER, text, line, column - text.length()));
}


std::unique_ptr<Token> Lexer::number() {
    std::cout << "number() at current=" << current << "\n" << std::flush;
    bool isFloat = false;
    while (isDigit(peek())) advance();
    if (peek() == '.' && isDigit(peekNext())) {
        isFloat = true;
        advance();
        while (isDigit(peek())) advance();
    }
    if ((peek() == 'e' || peek() == 'E') &&
        (isDigit(peekNext()) || 
         ((peekNext() == '+' || peekNext() == '-') && 
          current + 2 < source.length() && 
          isDigit(source[current + 2])))) {
        isFloat = true;
        advance();
        if (peek() == '+' || peek() == '-') advance();
        while (isDigit(peek())) advance();
    }
    
    std::string text = currentLexeme();
    std::cout << "Number text: '" << text << "'\n" << std::flush;
    auto token = std::unique_ptr<Token>(new Token(isFloat ? TokenType::FLOAT : TokenType::INTEGER, text, line, column - text.length()));
    try {
        if (isFloat) token->set_float_value(std::stod(text));
        else token->set_int_value(std::stoll(text));
    } catch (const std::exception& e) {
        std::cout << "Exception in number(): " << e.what() << "\n" << std::flush;
        return errorToken("Invalid number: " + text);
    }
    return token;
}

std::unique_ptr<Token> Lexer::string() {
    std::cout << "string() at current=" << current << "\n" << std::flush;
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { line++; column = 1; }
        if (peek() == '\\' && !isAtEnd()) advance();
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated string");
    advance();
    return std::unique_ptr<Token>(new Token(TokenType::STRING, currentLexeme(), line, column - currentLexeme().length()));
}

std::unique_ptr<Token> Lexer::character() {
    std::cout << "character() at current=" << current << "\n" << std::flush;
    bool hasEscape = false;
    if (peek() == '\\') {
        hasEscape = true;
        advance();
        advance();
    } else if (peek() != '\'') {
        advance();
    }
    if (peek() != '\'') return errorToken("Unterminated character literal");
    advance();
    auto token = std::unique_ptr<Token>(new Token(TokenType::CHARACTER, currentLexeme(), line, column - currentLexeme().length()));
    if (hasEscape) token->set_float_value(1.0); // Placeholder
    return token;
}


std::unique_ptr<Token> Lexer::errorToken(const std::string& message) {
    std::cout << "errorToken(): " << message << "\n" << std::flush;
    error_message = message;
    return std::unique_ptr<Token>(new Token(TokenType::ERROR, message, line, column));
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ': case '\r': case '\t': advance(); break;
            case '\n': line++; column = 1; advance(); break;
            default: return;
        }
    }
}

void Lexer::skipLineComment() {
    while (peek() != '\n' && !isAtEnd()) advance();
}

void Lexer::skipBlockComment() {
    advance();
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') { line++; column = 1; }
        advance();
    }
    if (isAtEnd()) { error_message = "Unterminated block comment"; return; }
    advance();
    advance();
}
