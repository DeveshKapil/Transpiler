// parser.hpp
#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "ast.hpp"
#include <memory>
#include <vector>
#include <string>
#include <fstream>

class Parser {
public:
    explicit Parser(std::vector<std::unique_ptr<Token>>&& tokens);
    ~Parser();
    std::unique_ptr<Program> parse();
    // std::unique_ptr<Program> parseProgram();

    // --- Log storage ---
    std::vector<std::string> parseLogs;
    const std::vector<std::string>& getParseLogs() const { return parseLogs; }

private:
    std::vector<std::unique_ptr<Token>> tokens; // Store all tokens
    size_t currentIndex = 0; // Index into tokens
    Token current;
    Token prev;
    std::ofstream logFile;

    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    bool expect(TokenType type, const std::string& errMsg);
    bool isAtEnd() ; // Returns true if current token is END_OF_FILE
    Token previous(); // Returns the previous token (if you keep track)
    // Top-level rules
    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<ASTNode> parseDeclaration();
    
    std::unique_ptr<ASTNode> parseType();

    // Declarations
    std::unique_ptr<ASTNode> parseFunctionDecl();
    std::unique_ptr<ASTNode> parseClassDecl();
    std::unique_ptr<ASTNode> parseStructDecl();
    std::unique_ptr<ASTNode> parseClassOrStructDecl(bool);
    std::unique_ptr<ASTNode> parseEnumDecl();
    std::unique_ptr<ASTNode> parseUnionDecl();
    std::unique_ptr<ASTNode> parseNamespaceDecl();
    std::unique_ptr<ASTNode> parseVariableDecl();
    std::unique_ptr<ASTNode> parseTypedefDecl();
    std::unique_ptr<ASTNode> parseUsingDirective();
    std::unique_ptr<ASTNode> parsePreprocessorDirective();
    std::unique_ptr<ASTNode> parseTemplateDecl();

    // Statements
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseIfStmt();
    std::unique_ptr<ASTNode> parseElseStmt();
    std::unique_ptr<ASTNode> parseSwitchStmt();
    std::unique_ptr<ASTNode> parseCaseStmt();
    std::unique_ptr<ASTNode> parseDefaultStmt();
    std::unique_ptr<ASTNode> parseWhileStmt();
    std::unique_ptr<ASTNode> parseDoWhileStmt();
    std::unique_ptr<ASTNode> parseForStmt();
    std::unique_ptr<ASTNode> parseReturnStmt();
    std::unique_ptr<ASTNode> parseBreakStmt();
    std::unique_ptr<ASTNode> parseContinueStmt();
    std::unique_ptr<ASTNode> parseGotoStmt();
    std::unique_ptr<ASTNode> parseTryStmt();
    std::unique_ptr<CatchStmt> parseCatchStmt();
    std::unique_ptr<ASTNode> parseThrowStmt();

    // Expressions
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseExpressionstmt();
    std::unique_ptr<ASTNode> parseBinary();
    std::unique_ptr<ASTNode> parseTernary();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseRelational();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseMultiplicative();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePostfix();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseAssignment();

    // Specialized constructs
    std::unique_ptr<ASTNode> parseStreamExpr();
    std::unique_ptr<ASTNode> parseFunctionCallSuffix(std::unique_ptr<ASTNode> callee);
    std::unique_ptr<ASTNode> parseTemplateTypeSuffix(std::string baseName);

    // Helpers
    bool isTypeToken(TokenType type);
    std::string tokenToString(TokenType type);
    std::unique_ptr<ASTNode> parseVariableDeclFromTokens(const Token& typeToken, const Token& nameToken);
    std::unique_ptr<ASTNode> parseFunctionDeclFromTokens(const Token& typeToken, const Token& nameToken);
    
};

#endif // PARSER_HPP
