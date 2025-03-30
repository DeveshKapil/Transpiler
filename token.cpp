#include "token.hpp"
#include <sstream>
#include <iomanip>
#include <array>

namespace {
static const std::array<std::pair<TokenType, const char*>, 92> token_type_names = {{
    {TokenType::END_OF_FILE, "END_OF_FILE"}, {TokenType::ERROR, "ERROR"},
    {TokenType::IDENTIFIER, "IDENTIFIER"}, {TokenType::INTEGER, "INTEGER"},
    {TokenType::FLOAT, "FLOAT"}, {TokenType::STRING, "STRING"},
    {TokenType::CHARACTER, "CHARACTER"}, {TokenType::INT, "INT"},
    {TokenType::VOID, "VOID"}, {TokenType::CHAR, "CHAR"},
    {TokenType::FLOAT_TYPE, "FLOAT_TYPE"}, {TokenType::DOUBLE, "DOUBLE"},
    {TokenType::BOOL, "BOOL"}, {TokenType::CLASS, "CLASS"},
    {TokenType::STRUCT, "STRUCT"}, {TokenType::ENUM, "ENUM"},
    {TokenType::UNION, "UNION"}, {TokenType::CONST, "CONST"},
    {TokenType::UNSIGNED, "UNSIGNED"}, {TokenType::SIGNED, "SIGNED"},
    {TokenType::SHORT, "SHORT"}, {TokenType::LONG, "LONG"},
    {TokenType::STATIC, "STATIC"}, {TokenType::EXTERN, "EXTERN"},
    {TokenType::REGISTER, "REGISTER"}, {TokenType::INLINE, "INLINE"},
    {TokenType::VIRTUAL, "VIRTUAL"}, {TokenType::EXPLICIT, "EXPLICIT"},
    {TokenType::FRIEND, "FRIEND"}, {TokenType::PRIVATE, "PRIVATE"},
    {TokenType::PUBLIC, "PUBLIC"}, {TokenType::PROTECTED, "PROTECTED"},
    {TokenType::IF, "IF"}, {TokenType::ELSE, "ELSE"},
    {TokenType::FOR, "FOR"}, {TokenType::WHILE, "WHILE"},
    {TokenType::DO, "DO"}, {TokenType::SWITCH, "SWITCH"},
    {TokenType::CASE, "CASE"}, {TokenType::DEFAULT, "DEFAULT"},
    {TokenType::BREAK, "BREAK"}, {TokenType::CONTINUE, "CONTINUE"},
    {TokenType::RETURN, "RETURN"}, {TokenType::GOTO, "GOTO"},
    {TokenType::PLUS, "PLUS"}, {TokenType::MINUS, "MINUS"},
    {TokenType::STAR, "STAR"}, {TokenType::SLASH, "SLASH"},
    {TokenType::PERCENT, "PERCENT"}, {TokenType::AMPERSAND, "AMPERSAND"},
    {TokenType::PIPE, "PIPE"}, {TokenType::CARET, "CARET"},
    {TokenType::TILDE, "TILDE"}, {TokenType::EXCLAIM, "EXCLAIM"},
    {TokenType::EQUAL, "EQUAL"}, {TokenType::LESS, "LESS"},
    {TokenType::GREATER, "GREATER"}, {TokenType::PLUS_EQUAL, "PLUS_EQUAL"},
    {TokenType::MINUS_EQUAL, "MINUS_EQUAL"}, {TokenType::STAR_EQUAL, "STAR_EQUAL"},
    {TokenType::SLASH_EQUAL, "SLASH_EQUAL"}, {TokenType::PERCENT_EQUAL, "PERCENT_EQUAL"},
    {TokenType::AND_EQUAL, "AND_EQUAL"}, {TokenType::OR_EQUAL, "OR_EQUAL"},
    {TokenType::XOR_EQUAL, "XOR_EQUAL"}, {TokenType::LESS_LESS, "LESS_LESS"},
    {TokenType::GREATER_GREATER, "GREATER_GREATER"}, {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
    {TokenType::NOT_EQUAL, "NOT_EQUAL"}, {TokenType::LESS_EQUAL, "LESS_EQUAL"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"}, {TokenType::AND_AND, "AND_AND"},
    {TokenType::OR_OR, "OR_OR"}, {TokenType::PLUS_PLUS, "PLUS_PLUS"},
    {TokenType::MINUS_MINUS, "MINUS_MINUS"}, {TokenType::ARROW, "ARROW"},
    {TokenType::LEFT_PAREN, "LEFT_PAREN"}, {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::LEFT_BRACE, "LEFT_BRACE"}, {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"}, {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::SEMICOLON, "SEMICOLON"}, {TokenType::COMMA, "COMMA"},
    {TokenType::DOT, "DOT"}, {TokenType::COLON, "COLON"},
    {TokenType::SCOPE, "SCOPE"}
}};

const char* getTokenTypeName(TokenType type) {
    for (size_t i = 0; i < token_type_names.size(); ++i) {
        if (token_type_names[i].first == type) {
            return token_type_names[i].second;
        }
    }
    return "UNKNOWN";
}
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(type=" << getTokenTypeName(type())
        << ", text='" << text() << "'"
        << ", line=" << line()
        << ", column=" << column();

    if (type() == TokenType::INTEGER) {
        oss << ", value=" << int_value();
    } else if (type() == TokenType::FLOAT) {
        oss << std::fixed << std::setprecision(6) << ", value=" << float_value();
    }

    oss << ")";
    return oss.str();
}