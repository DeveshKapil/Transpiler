#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include "lexer.hpp"
#include "ast.hpp"

class Parser {
public:
    explicit Parser(std::vector<std::unique_ptr<Token>> tokens);
    std::unique_ptr<ASTNode> parse();

private:
    std::vector<std::unique_ptr<Token>> tokens;
    size_t current;

    Token* peek() const;
    Token* advance();
    bool isAtEnd() const;
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);

    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseFunctionDecl();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseVarDecl();
    std::unique_ptr<ASTNode> parseIfStmt();
    std::unique_ptr<ASTNode> parseReturnStmt();
    std::unique_ptr<ASTNode> parseExpr();
    std::unique_ptr<ASTNode> parsePrimary();
};
#endif