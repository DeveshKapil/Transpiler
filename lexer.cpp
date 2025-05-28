#include "lexer.hpp"
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>

// === Constructor ===

Lexer::Lexer(const std::string& source)
    : source_(source), pos_(0), line_(1), column_(1), skipping_(false) {
    conditionalStack_.push_back(true);
}

// === Tokenization main loop ===

std::vector<std::unique_ptr<Token>> Lexer::tokenize() {
    std::vector<std::unique_ptr<Token>> tokens;
    while (true) {
        skipWhitespaceAndComments();

        if (pos_ >= source_.size()) {
            tokens.push_back(std::make_unique<Token>(TokenType::END_OF_FILE, "", line_, column_));
            break;
        }

        if (source_[pos_] == '#') {
            auto directiveToken = lexPreprocessorDirective();
            if (directiveToken) {
                // Preprocessor directive token (rare case)
                tokens.push_back(std::move(directiveToken));
            }
            continue;
        }

        if (skipping_) {
            // Skip tokens while skipping is active
            // Skip until whitespace or newline to avoid infinite loop
            while (pos_ < source_.size() && !isspace(source_[pos_])) {
                advance();
            }
            continue;
        }

        auto token = lexToken();
        if (token) {
            if (token->type() == TokenType::IDENTIFIER) {
                std::string expanded = expandMacro(token->text());
                if (expanded != token->text()) {
                    token = std::make_unique<Token>(TokenType::IDENTIFIER, expanded, token->line(), token->column());
                }
            }
            tokens.push_back(std::move(token));
        }
    }
    return tokens;
}

// === Basic helpers ===

char Lexer::peek() const {
    if (pos_ >= source_.size()) return '\0';
    return source_[pos_];
}

char Lexer::peekNext() const {
    if (pos_ + 1 >= source_.size()) return '\0';
    return source_[pos_ + 1];
}

char Lexer::advance() {
    if (pos_ >= source_.size()) return '\0';
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        if (isspace(c)) {
            advance();
        }
        else if (c == '/' && peekNext() == '/') {
            while (peek() != '\n' && peek() != '\0') advance();
        }
        else if (c == '/' && peekNext() == '*') {
            advance(); advance();
            while (!(peek() == '*' && peekNext() == '/') && peek() != '\0') advance();
            if (peek() == '*' && peekNext() == '/') {
                advance(); advance();
            }
        }
        else {
            break;
        }
    }
}

// === Token lexers ===

std::unique_ptr<Token> Lexer::lexToken() {
    char c = peek();

    if (isalpha(c) || c == '_') {
        return lexIdentifierOrKeyword();
    }
    else if (isdigit(c)) {
        return lexNumber();
    }
    else if (c == '"') {
        return lexString();
    }
    else if (c == '\'') {
        return lexChar();
    }

    advance(); // Consume the current character

    switch (c) {
        case '+':
            if (match('+')) return std::make_unique<Token>(TokenType::INCREMENT, "++", line_, column_ - 1);
            if (match('=')) return std::make_unique<Token>(TokenType::PLUS_EQUAL, "+=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::PLUS, "+", line_, column_ - 1);

        case '-':
            if (match('-')) return std::make_unique<Token>(TokenType::DECREMENT, "--", line_, column_ - 1);
            if (match('=')) return std::make_unique<Token>(TokenType::MINUS_EQUAL, "-=", line_, column_ - 1);
            if (match('>')) return std::make_unique<Token>(TokenType::ARROW, "->", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::MINUS, "-", line_, column_ - 1);

        case '=':
            if (match('=')) return std::make_unique<Token>(TokenType::EQUAL_EQUAL, "==", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::EQUAL, "=", line_, column_ - 1);

        case '!':
            if (match('=')) return std::make_unique<Token>(TokenType::NOT_EQUAL, "!=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::EXCLAIM, "!", line_, column_ - 1);

        case '<':
            if (match('=')) return std::make_unique<Token>(TokenType::LESS_EQUAL, "<=", line_, column_ - 1);
            if (match('<')) {
                if (match('=')) return std::make_unique<Token>(TokenType::LEFT_SHIFT_EQUAL, "<<=", line_, column_ - 2);
                return std::make_unique<Token>(TokenType::LESS_LESS, "<<", line_, column_ - 1);
            }
            return std::make_unique<Token>(TokenType::LESS, "<", line_, column_ - 1);

        case '>':
            if (match('=')) return std::make_unique<Token>(TokenType::GREATER_EQUAL, ">=", line_, column_ - 1);
            if (match('>')) {
                if (match('=')) return std::make_unique<Token>(TokenType::RIGHT_SHIFT_EQUAL, ">>=", line_, column_ - 2);
                return std::make_unique<Token>(TokenType::GREATER_GREATER, ">>", line_, column_ - 1);
            }
            return std::make_unique<Token>(TokenType::GREATER, ">", line_, column_ - 1);

        case '&':
            if (match('&')) return std::make_unique<Token>(TokenType::AND_AND, "&&", line_, column_ - 1);
            if (match('=')) return std::make_unique<Token>(TokenType::AND_EQUAL, "&=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::AMPERSAND, "&", line_, column_ - 1);

        case '|':
            if (match('|')) return std::make_unique<Token>(TokenType::OR_OR, "||", line_, column_ - 1);
            if (match('=')) return std::make_unique<Token>(TokenType::OR_EQUAL, "|=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::PIPE, "|", line_, column_ - 1);

        case '^':
            if (match('=')) return std::make_unique<Token>(TokenType::XOR_EQUAL, "^=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::CARET, "^", line_, column_ - 1);

        case '~':
            return std::make_unique<Token>(TokenType::TILDE, "~", line_, column_ - 1);

        case '*':
            if (match('=')) return std::make_unique<Token>(TokenType::STAR_EQUAL, "*=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::STAR, "*", line_, column_ - 1);

        case '/':
            if (match('=')) return std::make_unique<Token>(TokenType::SLASH_EQUAL, "/=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::SLASH, "/", line_, column_ - 1);

        case '%':
            if (match('=')) return std::make_unique<Token>(TokenType::PERCENT_EQUAL, "%=", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::PERCENT, "%", line_, column_ - 1);

        case '?':
            return std::make_unique<Token>(TokenType::QUESTION, "?", line_, column_ - 1);

        case ':':
            if (match(':')) return std::make_unique<Token>(TokenType::SCOPE, "::", line_, column_ - 1);
            return std::make_unique<Token>(TokenType::COLON, ":", line_, column_ - 1);

        case ';':
            return std::make_unique<Token>(TokenType::SEMICOLON, ";", line_, column_ - 1);

        case ',':
            return std::make_unique<Token>(TokenType::COMMA, ",", line_, column_ - 1);

        case '.':
            return std::make_unique<Token>(TokenType::DOT, ".", line_, column_ - 1);

        case '(':
            return std::make_unique<Token>(TokenType::LEFT_PAREN, "(", line_, column_ - 1);

        case ')':
            return std::make_unique<Token>(TokenType::RIGHT_PAREN, ")", line_, column_ - 1);

        case '{':
            return std::make_unique<Token>(TokenType::LEFT_BRACE, "{", line_, column_ - 1);

        case '}':
            return std::make_unique<Token>(TokenType::RIGHT_BRACE, "}", line_, column_ - 1);

        case '[':
            return std::make_unique<Token>(TokenType::LEFT_BRACKET, "[", line_, column_ - 1);

        case ']':
            return std::make_unique<Token>(TokenType::RIGHT_BRACKET, "]", line_, column_ - 1);

        case '#':
            return std::make_unique<Token>(TokenType::HASH, "#", line_, column_ - 1);

        default:
            return std::make_unique<Token>(TokenType::ERROR, std::string(1, c), line_, column_ - 1);
    }

    return std::make_unique<Token>(TokenType::ERROR, std::string(1, c), line_, column_ - 1);
}

std::unique_ptr<Token> Lexer::lexIdentifierOrKeyword() {
    int startLine = line_;
    int startCol = column_;
    std::string text;
    while (isalnum(peek()) || peek() == '_') {
        text.push_back(advance());
    }

    static const std::unordered_map<std::string, TokenType> keywords = {
    // C++ keywords
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
    {"goto", TokenType::GOTO}, {"namespace", TokenType::NAMESPACE}, {"using", TokenType::USING},
    {"template", TokenType::TEMPLATE},{"typedef", TokenType::TYPEDEF},

    // STL Containers
    {"vector", TokenType::VECTOR}, {"map", TokenType::MAP}, {"set", TokenType::SET},
    {"list", TokenType::LIST}, {"deque", TokenType::DEQUE}, {"unordered_map", TokenType::UNORDERED_MAP},
    {"unordered_set", TokenType::UNORDERED_SET}, {"multimap", TokenType::MULTIMAP},
    {"multiset", TokenType::MULTISET}, {"stack", TokenType::STACK}, {"queue", TokenType::QUEUE},
    {"priority_queue", TokenType::PRIORITY_QUEUE}, {"bitset", TokenType::BITSET},
    {"array", TokenType::ARRAY}, {"forward_list", TokenType::FORWARD_LIST},
    {"pair", TokenType::PAIR}, {"tuple", TokenType::TUPLE}, {"string", TokenType::STRING_LIB},
    {"optional", TokenType::OPTIONAL}, {"variant", TokenType::VARIANT}, {"any", TokenType::ANY},
    {"span", TokenType::SPAN}, {"valarray", TokenType::VALARRAY},

    // C standard lib functions
    {"printf", TokenType::PRINTF}, {"scanf", TokenType::SCANF}, {"malloc", TokenType::MALLOC},
    {"free", TokenType::FREE}, {"memcpy", TokenType::MEMCPY}, {"strcpy", TokenType::STRCPY},
    {"strlen", TokenType::STRLEN},

    // C++ I/O Streams
    {"cin", TokenType::CIN}, {"cout", TokenType::COUT}, {"cerr", TokenType::CERR}, {"clog", TokenType::CLIN},

    // Algorithms (add as needed)
    {"sort", TokenType::SORT}, {"find", TokenType::FIND}, {"count", TokenType::COUNT}, {"copy", TokenType::COPY},
    {"reverse", TokenType::REVERSE}, {"accumulate", TokenType::ACCUMULATE},
    {"all_of", TokenType::ALL_OF}, {"any_of", TokenType::ANY_OF}, {"none_of", TokenType::NONE_OF},
    {"lower_bound", TokenType::LOWER_BOUND}, {"upper_bound", TokenType::UPPER_BOUND},

    // Math (add as needed)
    {"abs", TokenType::ABS}, {"fabs", TokenType::FABS}, {"pow", TokenType::POW}, {"sqrt", TokenType::SQRT},
    {"sin", TokenType::SIN}, {"cos", TokenType::COS}, {"tan", TokenType::TAN},
    {"floor", TokenType::FLOOR}, {"ceil", TokenType::CEIL}, {"round", TokenType::ROUND},
    {"rand", TokenType::RAND}, {"srand", TokenType::SRAND}, {"exit", TokenType::EXIT},{"accumulate", TokenType::ACCUMULATE},
    

    // Strings (add as needed)
    {"stoi", TokenType::STOI}, {"stof", TokenType::STOF}, {"stod", TokenType::STOD}, {"to_string", TokenType::TO_STRING},
    {"strcmp", TokenType::STRCMP}, {"strncmp", TokenType::STRNCMP}, {"strchr", TokenType::STRCHR},
    {"strrchr", TokenType::STRRCHR}, {"strstr", TokenType::STRSTR}, {"strcat", TokenType::STRCAT},
    {"strncat", TokenType::STRNCAT},

    // Memory and allocation (add as needed)
    {"new", TokenType::NEW}, {"delete", TokenType::DELETE}, {"allocate", TokenType::ALLOCATE}, {"deallocate", TokenType::DEALLOCATE},

    // Time (add as needed)
    {"time", TokenType::TIME}, {"clock", TokenType::CLOCK}, {"difftime", TokenType::DIFFTIME},
    {"strftime", TokenType::STRFTIME}, {"localtime", TokenType::LOCALTIME}, {"gmtime", TokenType::GMTIME},

    // Concurrency (add as needed)
    {"thread", TokenType::THREAD}, {"mutex", TokenType::MUTEX}, {"lock_guard", TokenType::LOCK_GUARD},
    {"unique_lock", TokenType::UNIQUE_LOCK}, {"async", TokenType::ASYNC}, {"future", TokenType::FUTURE},
    {"promise", TokenType::PROMISE},

    // Exceptions (add as needed)
    {"try", TokenType::TRY}, {"catch", TokenType::CATCH}, {"throw", TokenType::THROW},
    {"exception", TokenType::EXCEPTION}, {"logic_error", TokenType::LOGIC_ERROR}, {"runtime_error", TokenType::RUNTIME_ERROR},

    // RTTI / Casting (add as needed)
    {"typeid", TokenType::TYPEID}, {"static_cast", TokenType::STATIC_CAST}, {"dynamic_cast", TokenType::DYNAMIC_CAST},
    {"const_cast", TokenType::CONST_CAST}, {"reinterpret_cast", TokenType::REINTERPRET_CAST}
};

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        if (text == "new" || text == "delete") {
            std::cerr << "[Lexer] Recognized keyword: '" << text << "' as token type " << static_cast<int>(it->second) << " at line " << startLine << ", col " << startCol << std::endl;
        }
        return std::make_unique<Token>(it->second, text, startLine, startCol);
    }
    if (text == "new" || text == "delete") {
        std::cerr << "[Lexer] '" << text << "' NOT recognized as keyword, returning IDENTIFIER at line " << startLine << ", col " << startCol << std::endl;
    }
    return std::make_unique<Token>(TokenType::IDENTIFIER, text, startLine, startCol);
}

std::unique_ptr<Token> Lexer::lexNumber() {
    int startLine = line_;
    int startCol = column_;
    std::string text;
    bool isFloat = false;

    while (isdigit(peek())) {
        text.push_back(advance());
    }
    if (peek() == '.') {
        isFloat = true;
        text.push_back(advance());
        while (isdigit(peek())) {
            text.push_back(advance());
        }
    }

    // TODO: Support exponent notation if desired

    if (isFloat) {
        return std::make_unique<Token>(TokenType::FLOAT, text, startLine, startCol);
    }
    return std::make_unique<Token>(TokenType::INTEGER, text, startLine, startCol);
}

std::unique_ptr<Token> Lexer::lexString() {
    int startLine = line_;
    int startCol = column_;
    std::string text;
    advance(); // skip opening "

    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            text.push_back(advance());
            if (peek() != '\0') text.push_back(advance());
        }
        else {
            text.push_back(advance());
        }
    }

    if (peek() == '"') {
        advance(); // skip closing "
        return std::make_unique<Token>(TokenType::STRING_LITERAL, text, startLine, startCol);
    }
    else {
        // Unterminated string literal error
        return std::make_unique<Token>(TokenType::ERROR, "Unterminated string literal", startLine, startCol);
    }
}

std::unique_ptr<Token> Lexer::lexChar() {
    int startLine = line_;
    int startCol = column_;
    std::string text;
    advance(); // consume opening '
    char c = peek();
    if (c == '\\') {
        text.push_back(advance()); // '\\'
        if (peek() != '\'') {
            text.push_back(advance()); // e.g., '\n', '\t', etc.
        }
    } else {
        text.push_back(advance());
    }
    if (peek() == '\'') {
        advance(); // consume closing '
    } else {
        // Unterminated char literal
        return std::make_unique<Token>(TokenType::ERROR, text, startLine, startCol);
    }
    return std::make_unique<Token>(TokenType::CHARACTER, text, startLine, startCol);
}

// === Macro system ===

void Lexer::defineMacro(const std::string& name, const std::string& value) {
    macros_[name] = value;
}

bool Lexer::isMacroDefined(const std::string& name) const {
    return macros_.find(name) != macros_.end();
}

std::string Lexer::expandMacro(const std::string& text) const {
    auto it = macros_.find(text);
    if (it != macros_.end()) {
        return it->second;
    }
    return text;
}

// === Preprocessor directives ===

std::unique_ptr<Token> Lexer::lexPreprocessorDirective() {
    // At start: pos_ points to '#'
    int startLine = line_;
    int startCol = column_;
    advance(); // consume '#'

    skipWhitespaceAndComments();

    // Read directive keyword
    std::string directive;
    while (isalnum(peek()) || peek() == '_') {
        directive.push_back(advance());
    }

    if (directive == "define") {
        skipWhitespaceAndComments();
        // Read macro name
        std::string macroName;
        while (isalnum(peek()) || peek() == '_') {
            macroName.push_back(advance());
        }
        skipWhitespaceAndComments();
        // Read macro replacement text (until newline)
        std::string macroValue;
        while (peek() != '\n' && peek() != '\0') {
            macroValue.push_back(advance());
        }
        defineMacro(macroName, macroValue);
        return std::make_unique<Token>(TokenType::PREPROCESSOR_DEFINE, macroName + " " + macroValue, startLine, startCol);
    }
    else if (directive == "undef") {
        skipWhitespaceAndComments();
        std::string macroName;
        while (isalnum(peek()) || peek() == '_') {
            macroName.push_back(advance());
        }
        macros_.erase(macroName);
        return std::make_unique<Token>(TokenType::PREPROCESSOR_UNDEF, macroName, startLine, startCol);
    }
    else if (directive == "pragma") {
        skipWhitespaceAndComments();
        std::string pragmaText;
        while (peek() != '\n' && peek() != '\0') {
            pragmaText.push_back(advance());
        }
        return std::make_unique<Token>(TokenType::PREPROCESSOR_PRAGMA, pragmaText, startLine, startCol);
    }
    else if (directive == "ifdef") {
        skipWhitespaceAndComments();
        std::string macroName;
        while (isalnum(peek()) || peek() == '_') {
            macroName.push_back(advance());
        }
        bool defined = isMacroDefined(macroName);
        conditionalStack_.push_back(defined && conditionalStack_.back());
        skipping_ = !conditionalStack_.back();
        return std::make_unique<Token>(TokenType::PREPROCESSOR_IFDEF, macroName, startLine, startCol);
    }
    else if (directive == "ifndef") {
        skipWhitespaceAndComments();
        std::string macroName;
        while (isalnum(peek()) || peek() == '_') {
            macroName.push_back(advance());
        }
        bool defined = !isMacroDefined(macroName);
        conditionalStack_.push_back(defined && conditionalStack_.back());
        skipping_ = !conditionalStack_.back();
        return std::make_unique<Token>(TokenType::PREPROCESSOR_IFNDEF, macroName, line_, column_);
    }
    else if (directive == "else") {
        if (conditionalStack_.empty()) {
            std::cerr << "Unexpected #else at line " << startLine << std::endl;
            return std::make_unique<Token>(TokenType::PREPROCESSOR_ELSE, "", line_, column_);
        }
        bool previous = conditionalStack_.back();
        conditionalStack_.pop_back();
        bool newVal = !previous && conditionalStack_.back();
        conditionalStack_.push_back(newVal);
        skipping_ = !conditionalStack_.back();
        return std::make_unique<Token>(TokenType::PREPROCESSOR_ELSE, "", line_, column_);
    }
    else if (directive == "endif") {
        if (conditionalStack_.empty()) {
            std::cerr << "Unexpected #endif at line " << startLine << std::endl;
            return std::make_unique<Token>(TokenType::PREPROCESSOR_ENDIF, "", line_, column_);
        }
        conditionalStack_.pop_back();
        if (conditionalStack_.empty()) {
            conditionalStack_.push_back(true);
        }
        skipping_ = !conditionalStack_.back();
        return std::make_unique<Token>(TokenType::PREPROCESSOR_ENDIF, "", line_, column_);
    }
    else if (directive == "if") {
        skipWhitespaceAndComments();
        // Parse rest of line as expression
        std::string expr;
        while (peek() != '\n' && peek() != '\0') {
            expr.push_back(advance());
        }
        int result = evalIfExpression(expr);
        conditionalStack_.push_back(result != 0 && conditionalStack_.back());
        skipping_ = !conditionalStack_.back();
        return std::make_unique<Token>(TokenType::PREPROCESSOR_IF, expr, startLine, startCol);
    }
    else if (directive == "elif") {
        if (conditionalStack_.empty()) {
            std::cerr << "Unexpected #elif at line " << startLine << std::endl;
            return std::make_unique<Token>(TokenType::PREPROCESSOR_ELIF, "", line_, column_);
        }
        // Pop previous condition, push new based on expression and parent's condition
        conditionalStack_.pop_back();

        skipWhitespaceAndComments();
        std::string expr;
        while (peek() != '\n' && peek() != '\0') {
            expr.push_back(advance());
        }
        int result = evalIfExpression(expr);
        bool parent = conditionalStack_.back();
        conditionalStack_.push_back(result != 0 && parent);
        skipping_ = !conditionalStack_.back();
        return std::make_unique<Token>(TokenType::PREPROCESSOR_ELIF, expr, startLine, startCol);
    }
    else if (directive == "include") {
        skipWhitespaceAndComments();
        // Handle #include "header.h"
        if (peek() == '"') {
            auto headerToken = lexString();
            return std::make_unique<Token>(TokenType::PREPROCESSOR_INCLUDE, headerToken->text(), startLine, startCol);
        }
        // Handle #include <header>
        else if (peek() == '<') {
            advance(); // skip '<'
            std::string header;
            while (peek() != '>' && peek() != '\0') {
                header.push_back(advance());
            }
            if (peek() == '>') advance();
            return std::make_unique<Token>(TokenType::PREPROCESSOR_INCLUDE, "<" + header + ">", startLine, startCol);
        }
        else {
            // Error: expected " or <
            return std::make_unique<Token>(TokenType::ERROR, "Malformed #include directive", startLine, startCol);
        }
    }
    else {
        // Unknown directive: capture rest of line
        std::string unknownText;
        while (peek() != '\n' && peek() != '\0') {
            unknownText.push_back(advance());
        }
        return std::make_unique<Token>(TokenType::PREPROCESSOR_UNKNOWN, directive + " " + unknownText, startLine, startCol);
    }
}

// === Expression evaluator for #if ===

void Lexer::skipSpaces(const std::string& expr, size_t& idx) {
    while (idx < expr.size() && isspace(expr[idx])) idx++;
}

bool Lexer::matchKeyword(const std::string& expr, size_t& idx, const std::string& keyword) {
    size_t len = keyword.size();
    if (expr.compare(idx, len, keyword) == 0) {
        idx += len;
        return true;
    }
    return false;
}

// int Lexer::evalIfExpression(const std::string& expr) {
//     size_t idx = 0;
//     try {
//         int val = preparseExpression(expr, idx);
//         skipSpaces(expr, idx);
//         if (idx != expr.size()) {
//             std::cerr << "Warning: extra tokens after expression in #if" << std::endl;
//         }
//         return val;
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Error evaluating #if expression: " << e.what() << std::endl;
//         return 0;
//     }
// }
int Lexer::evalIfExpression(const std::string& expr) {
    size_t idx = 0;

    // Basic recursive-descent parser for expressions
    // Supports: integers, +, -, *, /, %, parentheses, defined(X)

    std::function<int()> parseExpression;
    std::function<int()> parseTerm;
    std::function<int()> parseFactor;

    auto skipSpaces = [&](void) {
        while (idx < expr.size() && isspace(expr[idx])) idx++;
    };

    auto matchChar = [&](char ch) -> bool {
        skipSpaces();
        if (idx < expr.size() && expr[idx] == ch) {
            idx++;
            return true;
        }
        return false;
    };

    auto parseNumber = [&]() -> int {
        skipSpaces();
        int sign = 1;
        if (matchChar('-')) sign = -1;
        else if (matchChar('+')) sign = 1;

        int value = 0;
        while (idx < expr.size() && isdigit(expr[idx])) {
            value = value * 10 + (expr[idx++] - '0');
        }
        return sign * value;
    };

    parseFactor = [&]() -> int {
        skipSpaces();
        if (matchChar('(')) {
            int val = parseExpression();
            matchChar(')');
            return val;
        } else if (expr.compare(idx, 7, "defined") == 0) {
            idx += 7;
            skipSpaces();
            std::string macroName;

            if (matchChar('(')) {
                while (idx < expr.size() && (isalnum(expr[idx]) || expr[idx] == '_')) {
                    macroName += expr[idx++];
                }
                matchChar(')');
            } else {
                while (idx < expr.size() && (isalnum(expr[idx]) || expr[idx] == '_')) {
                    macroName += expr[idx++];
                }
            }
            return isMacroDefined(macroName) ? 1 : 0;
        } else {
            return parseNumber();
        }
    };

    parseTerm = [&]() -> int {
        int val = parseFactor();
        while (true) {
            if (matchChar('*')) val *= parseFactor();
            else if (matchChar('/')) val /= parseFactor();
            else if (matchChar('%')) val %= parseFactor();
            else break;
        }
        return val;
    };

    parseExpression = [&]() -> int {
        int val = parseTerm();
        while (true) {
            if (matchChar('+')) val += parseTerm();
            else if (matchChar('-')) val -= parseTerm();
            else break;
        }
        return val;
    };

    try {
        int result = parseExpression();
        return result;
    } catch (...) {
        std::cerr << "Error evaluating #if expression: " << expr << std::endl;
        return 0;
    }
}

// Grammar and precedence:

// expression -> logical_or
// logical_or -> logical_and ('||' logical_and)*
// logical_and -> equality ('&&' equality)*
// equality -> relational (('==' | '!=') relational)*
// relational -> unary (('<' | '>' | '<=' | '>=') unary)*
// unary -> ('!' | '-' | '+') unary | primary
// primary -> INTEGER | defined(identifier) | '(' expression ')'

// Parsing helpers:

int Lexer::preparseExpression(const std::string& expr, size_t& idx) {
    return preparseLogicalOr(expr, idx);
}

int Lexer::preparseLogicalOr(const std::string& expr, size_t& idx) {
    int left = preparseLogicalAnd(expr, idx);
    while (true) {
        skipSpaces(expr, idx);
        if (expr.compare(idx, 2, "||") == 0) {
            idx += 2;
            int right = preparseLogicalAnd(expr, idx);
            left = (left || right) ? 1 : 0;
        }
        else {
            break;
        }
    }
    return left;
}

int Lexer::preparseLogicalAnd(const std::string& expr, size_t& idx) {
    int left = preparseEquality(expr, idx);
    while (true) {
        skipSpaces(expr, idx);
        if (expr.compare(idx, 2, "&&") == 0) {
            idx += 2;
            int right = preparseEquality(expr, idx);
            left = (left && right) ? 1 : 0;
        }
        else {
            break;
        }
    }
    return left;
}

int Lexer::preparseEquality(const std::string& expr, size_t& idx) {
    int left = preparseRelational(expr, idx);
    while (true) {
        skipSpaces(expr, idx);
        if (expr.compare(idx, 2, "==") == 0) {
            idx += 2;
            int right = preparseRelational(expr, idx);
            left = (left == right) ? 1 : 0;
        }
        else if (expr.compare(idx, 2, "!=") == 0) {
            idx += 2;
            int right = preparseRelational(expr, idx);
            left = (left != right) ? 1 : 0;
        }
        else {
            break;
        }
    }
    return left;
}

int Lexer::preparseRelational(const std::string& expr, size_t& idx) {
    int left = preparseUnary(expr, idx);
    while (true) {
        skipSpaces(expr, idx);
        if (expr.compare(idx, 2, "<=") == 0) {
            idx += 2;
            int right = preparseUnary(expr, idx);
            left = (left <= right) ? 1 : 0;
        }
        else if (expr.compare(idx, 2, ">=") == 0) {
            idx += 2;
            int right = preparseUnary(expr, idx);
            left = (left >= right) ? 1 : 0;
        }
        else if (expr.compare(idx, 1, "<") == 0) {
            idx += 1;
            int right = preparseUnary(expr, idx);
            left = (left < right) ? 1 : 0;
        }
        else if (expr.compare(idx, 1, ">") == 0) {
            idx += 1;
            int right = preparseUnary(expr, idx);
            left = (left > right) ? 1 : 0;
        }
        else {
            break;
        }
    }
    return left;
}

int Lexer::preparseUnary(const std::string& expr, size_t& idx) {
    skipSpaces(expr, idx);
    if (expr.compare(idx, 1, "!") == 0) {
        idx++;
        int val = preparseUnary(expr, idx);
        return !val ? 1 : 0;
    }
    else if (expr.compare(idx, 1, "-") == 0) {
        idx++;
        int val = preparseUnary(expr, idx);
        return -val;
    }
    else if (expr.compare(idx, 1, "+") == 0) {
        idx++;
        return preparseUnary(expr, idx);
    }
    else {
        return preparsePrimary(expr, idx);
    }
}

int Lexer::preparsePrimary(const std::string& expr, size_t& idx) {
    skipSpaces(expr, idx);
    if (idx >= expr.size()) throw std::runtime_error("Unexpected end of expression");

    // Handle (expr)
    if (expr[idx] == '(') {
        idx++;
        int val = preparseExpression(expr, idx);
        skipSpaces(expr, idx);
        if (idx >= expr.size() || expr[idx] != ')') {
            throw std::runtime_error("Expected ')'");
        }
        idx++;
        return val;
    }

    // Handle defined identifier
    if (matchKeyword(expr, idx, "defined")) {
        skipSpaces(expr, idx);
        bool hasParen = false;
        if (idx < expr.size() && expr[idx] == '(') {
            hasParen = true;
            idx++;
            skipSpaces(expr, idx);
        }
        std::string macroName;
        while (idx < expr.size() && (isalnum(expr[idx]) || expr[idx] == '_')) {
            macroName.push_back(expr[idx++]);
        }
        if (hasParen) {
            skipSpaces(expr, idx);
            if (idx >= expr.size() || expr[idx] != ')') {
                throw std::runtime_error("Expected ')' after defined");
            }
            idx++;
        }
        return isMacroDefined(macroName) ? 1 : 0;
    }

    // Parse number literal
    if (isdigit(expr[idx])) {
        size_t start = idx;
        while (idx < expr.size() && isdigit(expr[idx])) idx++;
        if (start == idx) {
            throw std::runtime_error("Expected digit in #if expression");
        }
        int val = std::stoi(expr.substr(start, idx - start));
        return val;
    }

    // Otherwise error
    throw std::runtime_error(std::string("Unexpected token in #if expression at position ") + std::to_string(idx));
}
