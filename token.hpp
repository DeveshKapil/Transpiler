#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <memory>

enum class TokenType {
    END_OF_FILE, ERROR,
    IDENTIFIER, INTEGER, FLOAT, STRING, CHARACTER,
    INT, VOID, CHAR, FLOAT_TYPE, DOUBLE, BOOL, CLASS, STRUCT, ENUM, UNION,
    CONST, UNSIGNED, SIGNED, SHORT, LONG, STATIC, EXTERN, REGISTER, INLINE,
    VIRTUAL, EXPLICIT, FRIEND, PRIVATE, PUBLIC, PROTECTED,
    IF, ELSE, FOR, WHILE, DO, SWITCH, CASE, DEFAULT, BREAK, CONTINUE, RETURN, GOTO,
    PLUS, MINUS, STAR, SLASH, PERCENT, AMPERSAND, PIPE, CARET, TILDE, EXCLAIM,
    EQUAL, LESS, GREATER,
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL, AND_EQUAL,
    OR_EQUAL, XOR_EQUAL, LESS_LESS, GREATER_GREATER, EQUAL_EQUAL, NOT_EQUAL,
    LESS_EQUAL, GREATER_EQUAL, AND_AND, OR_OR, PLUS_PLUS, MINUS_MINUS, ARROW,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    SEMICOLON, COMMA, DOT, COLON, SCOPE , HASH
};

class Token {
public:
    Token(TokenType type, std::string text, int line, int column)
        : type_(type), text_(std::move(text)), line_(line), column_(column),
          int_value_(0), is_float_(false), has_escape_(false) {}

    TokenType type() const { return type_; }
    const std::string& text() const { return text_; }
    int line() const { return line_; }
    int column() const { return column_; }
    long long int_value() const { return int_value_; }
    double float_value() const { return float_value_; }
    bool is_float() const { return is_float_; }
    bool has_escape() const { return has_escape_; }

    void set_int_value(long long value) { int_value_ = value; is_float_ = false; }
    void set_float_value(double value) { float_value_ = value; is_float_ = true; }

    std::string toString() const;

private:
    TokenType type_;
    std::string text_;
    int line_;
    int column_;
    union {
        long long int_value_;
        double float_value_;
    };
    bool is_float_;
    bool has_escape_;
};
#endif