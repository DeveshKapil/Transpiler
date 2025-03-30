#include "parser.hpp"
#include <stdexcept>
#include <iostream>

Parser::Parser(std::vector<std::unique_ptr<Token>> tokens) 
    : tokens(std::move(tokens)), current(0) {
    std::cout << "Parser constructed with " << this->tokens.size() << " tokens\n";
}

Token* Parser::peek() const {
    if (isAtEnd()) return nullptr;
    return tokens[current].get();
}

Token* Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1].get();
}

bool Parser::isAtEnd() const {
    return current >= tokens.size() || tokens[current]->type() == TokenType::END_OF_FILE;
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    if (tokens[current]->type() == type) {
        advance();
        std::cout << "Matched token: " << tokens[current - 1]->toString() << "\n";
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (match(type)) return;
    Token* nextToken = peek();
    if (nextToken == nullptr) {
        throw std::runtime_error(message + " at end of input");
    }
    throw std::runtime_error(message + " at line " + std::to_string(nextToken->line()));
}

std::unique_ptr<ASTNode> Parser::parse() {
    std::cout << "Starting parse()\n";
    return parseProgram();
}
std::unique_ptr<ASTNode> Parser::parseProgram() {
    std::cout << "Parsing program\n";
    auto func = parseFunctionDecl();
    if (!isAtEnd()) {
        consume(TokenType::END_OF_FILE, "Expected end of file");
    }
    return func;
}

std::unique_ptr<ASTNode> Parser::parseFunctionDecl() {
    std::cout << "Parsing function declaration\n";
    std::string returnType = advance()->text();
    std::string name = advance()->text();
    std::cout << "Function: " << returnType << " " << name << "\n";
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    consume(TokenType::RIGHT_PAREN, "Expected ')' after '('");
    consume(TokenType::LEFT_BRACE, "Expected '{' after function declaration");

    std::vector<std::unique_ptr<ASTNode>> statements;
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }

    auto func = std::unique_ptr<FunctionDecl>(new FunctionDecl(returnType, name));
    func->statements = std::move(statements);
    std::cout << "FunctionDecl created with " << func->statements.size() << " statements\n";
    return std::unique_ptr<ASTNode>(func.release());
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    std::cout << "Parsing statement at token " << current << "\n";
    if (match(TokenType::INT) || match(TokenType::FLOAT_TYPE)) {
        current--;
        return parseVarDecl();
    }
    if (match(TokenType::IF)) return parseIfStmt();
    if (match(TokenType::RETURN)) return parseReturnStmt();
    throw std::runtime_error("Unexpected token at line " + std::to_string(peek()->line()));
}

std::unique_ptr<ASTNode> Parser::parseVarDecl() {
    std::cout << "Parsing variable declaration\n";
    std::string type = advance()->text();
    std::string name = advance()->text();
    consume(TokenType::EQUAL, "Expected '=' after variable name");
    auto initializer = parseExpr();
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return std::unique_ptr<ASTNode>(new VarDecl(type, name, std::move(initializer)));
}

std::unique_ptr<ASTNode> Parser::parseIfStmt() {
    std::cout << "Parsing if statement\n";
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = parseExpr();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    consume(TokenType::LEFT_BRACE, "Expected '{' after condition");

    std::vector<std::unique_ptr<ASTNode>> thenBranch;
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        thenBranch.push_back(parseStatement());
    }

    return std::unique_ptr<ASTNode>(new IfStmt(std::move(condition), std::move(thenBranch)));
}

std::unique_ptr<ASTNode> Parser::parseReturnStmt() {
    std::cout << "Parsing return statement\n";
    auto expr = parseExpr();
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    return std::unique_ptr<ASTNode>(new ReturnStmt(std::move(expr)));
}

std::unique_ptr<ASTNode> Parser::parseExpr() {
    std::cout << "Parsing expression\n";
    auto left = parsePrimary();
    if (match(TokenType::GREATER)) {
        auto right = parsePrimary();
        return std::unique_ptr<ASTNode>(new BinaryExpr(">", std::move(left), std::move(right)));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    std::cout << "Parsing primary at token " << current << "\n";
    Token* token = advance();
    if (token->type() == TokenType::IDENTIFIER) {
        return std::unique_ptr<ASTNode>(new Identifier(token->text()));
    }
    if (token->type() == TokenType::INTEGER) {
        return std::unique_ptr<ASTNode>(new Literal(token->text(), "int"));
    }
    if (token->type() == TokenType::FLOAT) {
        return std::unique_ptr<ASTNode>(new Literal(token->text(), "float"));
    }
    throw std::runtime_error("Unexpected token in expression at line " + std::to_string(token->line()));
}