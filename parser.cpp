#include "parser.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>


// --- Constructor ---
Parser::Parser(std::vector<std::unique_ptr<Token>>&& tokens)
    : tokens(std::move(tokens)), currentIndex(0) {
    
    if (!this->tokens.empty()) {
        current = *this->tokens[0];
    }
    // Initialize log storage
    parseLogs.clear();
    logFile.open("OUTPUT/parser_logs.txt", std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open OUTPUT/parser_logs.txt for writing");
    }
    logFile<< "[Parser] Log file opened successfully." << std::endl;
}
Parser::~Parser() {
    
    logFile.flush();
    if (logFile.is_open()) {
        logFile.close();
    }
}

// --- Token helpers ---
void Parser::advance() {
    
    prev = current;
    if (currentIndex + 1 < tokens.size()) {
        currentIndex++;
        current = *tokens[currentIndex];
    } else {
        // Stay at END_OF_FILE
        current = *tokens.back();
    }
    // Store the token log instead of printing
    // parseLogs.push_back("[Parser] : " + current.toString());
    logFile << "[Parser] : " << current.toString() << std::endl;
    std::cout << "[Parser] : ";
    std::cout << current.toString() << std::endl;
    logFile.flush();
}

bool Parser::match(TokenType type) {
    if (current.type() == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    return current.type() == type;
}

bool Parser::isAtEnd() {
    return current.type() == TokenType::END_OF_FILE;
}

Token Parser::previous() {
    return prev;
}

bool Parser::expect(TokenType type, const std::string& errMsg) {
    if (current.type() == type) {
        advance();
        return true;
    }
    std::ostringstream oss;
    oss << errMsg << " (at line " << current.line() << ", column " << current.column() << ", token: '" << current.text() << "')" ;
    throw std::runtime_error(oss.str());
}

// --- Top-level parse ---
std::unique_ptr<Program> Parser::parse() {
    return parseProgram();
}

std::unique_ptr<Program> Parser::parseProgram() {
    std::cout << "[DEBUG] :: parseProgram" << std::endl;
    auto program = std::make_unique<Program>();
    while (current.type() != TokenType::END_OF_FILE) {
        auto decl = parseDeclaration();
        if (!decl) continue; // Skip null declarations, keep parsing
        program->globals.push_back(std::move(decl));
    }
    return program;
}

// --- Declarations ---
std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    std::cout << "[DEBUG] :: Declaration" << std::endl;
    // Handle preprocessor directives (HASH and related)
    if (current.type() == TokenType::HASH) {
        // Always call parsePreprocessorDirective to build AST node for preprocessor lines
        return parsePreprocessorDirective();
    }
    // If we see a preprocessor directive token (not just HASH), skip it robustly
    if (
        current.type() == TokenType::PREPROCESSOR_INCLUDE ||
        current.type() == TokenType::PREPROCESSOR_DEFINE ||
        current.type() == TokenType::PREPROCESSOR_IFDEF ||
        current.type() == TokenType::PREPROCESSOR_IFNDEF ||
        current.type() == TokenType::PREPROCESSOR_IF ||
        current.type() == TokenType::PREPROCESSOR_ELIF ||
        current.type() == TokenType::PREPROCESSOR_ELSE ||
        current.type() == TokenType::PREPROCESSOR_ENDIF ||
        current.type() == TokenType::PREPROCESSOR_UNDEF ||
        current.type() == TokenType::PREPROCESSOR_PRAGMA ||
        current.type() == TokenType::PREPROCESSOR_UNKNOWN
    ) {
        // partial implimentaion
        advance();
        return nullptr;
    }

    // Handle C++ member modifiers
    bool isVirtual = false, isStatic = false, isInline = false, isConstexpr = false, isFriend = false;
    while (true) {
        if (current.type() == TokenType::VIRTUAL) {
            isVirtual = true;
            advance();
        } else if (current.type() == TokenType::STATIC) {
            isStatic = true;
            advance();
        } else if (current.type() == TokenType::INLINE) {
            isInline = true;
            advance();
        } else if (current.type() == TokenType::CONST) {
            isConstexpr = true;
            advance();
        } else if (current.type() == TokenType::FRIEND) {
            isFriend = true;
            advance();
        } else {
            break;
        }
    }

    if (check(TokenType::TEMPLATE)) return parseTemplateDecl();
    if (check(TokenType::CLASS)) return parseClassDecl();
    if (check(TokenType::STRUCT)) return parseStructDecl();
    if (check(TokenType::ENUM)) return parseEnumDecl();
    if (check(TokenType::UNION)) return parseUnionDecl();
    if (check(TokenType::NAMESPACE)) return parseNamespaceDecl();
    if (check(TokenType::TYPEDEF)) return parseTypedefDecl();
    if (check(TokenType::USING)) return parseUsingDirective();

    if (isTypeToken(current.type())) {
        // Peek ahead to see if this is a function or variable declaration
        Token typeToken = current;
        Token prevToken = prev;
        advance();
        if (current.type() == TokenType::IDENTIFIER) {
            Token nameToken = current;
            advance();
            if (current.type() == TokenType::LEFT_PAREN) {
                // Function declaration
                auto funcNode = parseFunctionDeclFromTokens(typeToken, nameToken);
                // Set modifiers
                if (isVirtual) static_cast<FunctionDecl*>(funcNode.get())->isVirtual = true;
                if (isStatic) static_cast<FunctionDecl*>(funcNode.get())->isStatic = true;
                if (isInline) static_cast<FunctionDecl*>(funcNode.get())->isInline = true;
                if (isConstexpr) static_cast<FunctionDecl*>(funcNode.get())->isConst = true;
                if (isFriend) static_cast<FunctionDecl*>(funcNode.get())->isFriend = true;
                return funcNode;
            } else {
                // Variable declaration
                auto varNode = parseVariableDeclFromTokens(typeToken, nameToken);
                // Set modifiers if you add them to VarDecl
                return varNode;
            }
        } else {
            // Rewind if not a valid declaration
            current = typeToken;
            prev = prevToken;
            // If we hit a '}' or EOF, stop parsing
            if (current.type() == TokenType::RIGHT_BRACE || current.type() == TokenType::END_OF_FILE) {
                return nullptr;
            }
        }
    }
    if (current.type() == TokenType::RIGHT_BRACE || current.type() == TokenType::END_OF_FILE) {
        return nullptr;
    }
    return parseStatement();
}

bool Parser::isTypeToken(TokenType type) {
    switch (type) {
        case TokenType::INT:
        case TokenType::FLOAT_TYPE:
        case TokenType::DOUBLE:
        case TokenType::CHAR:
        case TokenType::BOOL:
        case TokenType::VOID:
        case TokenType::IDENTIFIER: // for user-defined types
        case TokenType::VECTOR:
        case TokenType::MAP:
        case TokenType::SET:
        case TokenType::UNORDERED_MAP:
        case TokenType::UNORDERED_SET:
        case TokenType::MULTIMAP:
        case TokenType::MULTISET:
        case TokenType::STACK:
        case TokenType::QUEUE:
        case TokenType::PRIORITY_QUEUE:
        case TokenType::BITSET:
        case TokenType::ARRAY:
        case TokenType::LIST:
        case TokenType::FORWARD_LIST:
        case TokenType::PAIR:
        case TokenType::TUPLE:
        case TokenType::STRING_LIB:
        case TokenType::OPTIONAL:
        case TokenType::VARIANT:
        case TokenType::ANY:
        case TokenType::SPAN:
        case TokenType::VALARRAY:
            return true;
        default:
            return false;
    }
}

// --- Example: Class Declaration ---

// ...existing code...

std::unique_ptr<ASTNode> Parser::parseClassDecl() {
    expect(TokenType::CLASS, "Expected 'class' keyword");
    expect(TokenType::IDENTIFIER, "Expected class name");
    std::string name = previous().text();
    std::vector<BaseSpecifier> bases;
    // Parse inheritance list
    if (match(TokenType::COLON)) {
        do {
            std::string access = "private"; // default for class
            if (check(TokenType::PUBLIC) || check(TokenType::PROTECTED) || check(TokenType::PRIVATE)) {
                access = current.text();
                advance();
            }
            expect(TokenType::IDENTIFIER, "Expected base class name");
            std::string baseName = previous().text();
            bases.emplace_back(baseName, access);
        } while (match(TokenType::COMMA));
    }
    expect(TokenType::LEFT_BRACE, "Expected '{' after class name");
    auto classNode = std::make_unique<ClassDecl>(name);
    classNode->bases = std::move(bases);
    enum Access { PUBLIC, PRIVATE, PROTECTED };
    Access currentAccess = PRIVATE;
    while (current.type() != TokenType::RIGHT_BRACE && current.type() != TokenType::END_OF_FILE) {
        if (match(TokenType::PUBLIC)) { expect(TokenType::COLON, "Expected ':' after 'public'"); currentAccess = PUBLIC; continue; }
        if (match(TokenType::PRIVATE)) { expect(TokenType::COLON, "Expected ':' after 'private'"); currentAccess = PRIVATE; continue; }
        if (match(TokenType::PROTECTED)) { expect(TokenType::COLON, "Expected ':' after 'protected'"); currentAccess = PROTECTED; continue; }
        auto member = parseDeclaration();
        if (!member) continue;
        if (currentAccess == PUBLIC) classNode->publicMembers.push_back(std::move(member));
        else if (currentAccess == PROTECTED) classNode->protectedMembers.push_back(std::move(member));
        else classNode->privateMembers.push_back(std::move(member));
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' after class body");
    expect(TokenType::SEMICOLON, "Expected ';' after class declaration");
    return classNode;
}

std::unique_ptr<ASTNode> Parser::parseStructDecl() {
    expect(TokenType::STRUCT, "Expected 'struct' keyword");
    expect(TokenType::IDENTIFIER, "Expected struct name");
    std::string name = previous().text();
    std::vector<BaseSpecifier> bases;
    // Parse inheritance list
    if (match(TokenType::COLON)) {
        do {
            std::string access = "public"; // default for struct
            if (check(TokenType::PUBLIC) || check(TokenType::PROTECTED) || check(TokenType::PRIVATE)) {
                access = current.text();
                advance();
            }
            expect(TokenType::IDENTIFIER, "Expected base struct/class name");
            std::string baseName = previous().text();
            bases.emplace_back(baseName, access);
        } while (match(TokenType::COMMA));
    }
    expect(TokenType::LEFT_BRACE, "Expected '{' after struct name");
    auto structNode = std::make_unique<StructDecl>(name);
    structNode->bases = std::move(bases);
    enum Access { PUBLIC, PRIVATE, PROTECTED };
    Access currentAccess = PUBLIC;
    while (current.type() != TokenType::RIGHT_BRACE && current.type() != TokenType::END_OF_FILE) {
        if (match(TokenType::PUBLIC)) { expect(TokenType::COLON, "Expected ':' after 'public'"); currentAccess = PUBLIC; continue; }
        if (match(TokenType::PRIVATE)) { expect(TokenType::COLON, "Expected ':' after 'private'"); currentAccess = PRIVATE; continue; }
        if (match(TokenType::PROTECTED)) { expect(TokenType::COLON, "Expected ':' after 'protected'"); currentAccess = PROTECTED; continue; }
        auto member = parseDeclaration();
        if (!member) continue;
        if (currentAccess == PUBLIC) structNode->publicMembers.push_back(std::move(member));
        else if (currentAccess == PROTECTED) structNode->protectedMembers.push_back(std::move(member));
        else structNode->privateMembers.push_back(std::move(member));
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' after struct body");
    expect(TokenType::SEMICOLON, "Expected ';' after struct declaration");
    return structNode;
}

// ...existing code...
// --- Example: Variable Declaration ---
std::unique_ptr<ASTNode> Parser::parseVariableDecl() {
    std::cout << "[DEBUG] :: VariableDecl" << std::endl;
    // Parse base type identifier
    std::string typeName = current.text();
    advance();
    // Check for template type (e.g., vector<int>)
    std::unique_ptr<ASTNode> typeNode = std::make_unique<Identifier>(typeName);
    if (current.type() == TokenType::LESS) { // '<'
        // Use parseType to handle template arguments and nesting
        currentIndex -= 1;
        current = *tokens[currentIndex];
        typeNode = parseType();
    }
    // Handle pointer/reference tokens between type and variable name
    while (current.type() == TokenType::STAR || current.type() == TokenType::AMPERSAND) {
        if (current.type() == TokenType::STAR) {
            typeNode = std::make_unique<PointerType>(std::move(typeNode));
        } else if (current.type() == TokenType::AMPERSAND) {
            typeNode = std::make_unique<ReferenceType>(std::move(typeNode));
        }
        advance();
    }
    // Parse first variable
    expect(TokenType::IDENTIFIER, "Expected variable name");
    std::vector<std::string> names;
    std::vector<std::unique_ptr<ASTNode>> initializers;
    names.push_back(previous().text());
    std::unique_ptr<ASTNode> firstInit = nullptr;
    if (match(TokenType::EQUAL)) {
        firstInit = parseExpression();
        // Accept optional 'f' identifier after float literal (e.g., 3.14f)
        if (typeNode->type == ASTNodeType::IDENTIFIER) {
            std::string tname = static_cast<Identifier*>(typeNode.get())->name;
            if (tname == "float" && current.type() == TokenType::IDENTIFIER && current.text() == "f") {
                advance();
            }
        }
    }
    initializers.push_back(std::move(firstInit));
    // Parse additional variables separated by commas
    while (match(TokenType::COMMA)) {
        expect(TokenType::IDENTIFIER, "Expected variable name after ','");
        names.push_back(previous().text());
        std::unique_ptr<ASTNode> nextInit = nullptr;
        if (match(TokenType::EQUAL)) {
            nextInit = parseExpression();
            if (typeNode->type == ASTNodeType::IDENTIFIER) {
                std::string tname = static_cast<Identifier*>(typeNode.get())->name;
                if (tname == "float" && current.type() == TokenType::IDENTIFIER && current.text() == "f") {
                    advance();
                }
            }
        }
        initializers.push_back(std::move(nextInit));
    }
    expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    // If only one variable, emit as before
    if (names.size() == 1) {
        auto varNode = std::make_unique<VarDecl>(names[0]);
        varNode->type = std::move(typeNode);
        varNode->initializer = std::move(initializers[0]);
        return varNode;
    } else {
        // Multi-variable declaration: emit a BlockStmt containing separate VarDecls
        auto block = std::make_unique<BlockStmt>();
        for (size_t i = 0; i < names.size(); ++i) {
            auto varNode = std::make_unique<VarDecl>(names[i]);
            varNode->type = std::make_unique<Identifier>(static_cast<Identifier*>(typeNode.get())->name);
            varNode->initializer = std::move(initializers[i]);
            block->statements.push_back(std::move(varNode));
        }
        return block;
    }
}
// Helper for parseDeclaration only
std::unique_ptr<ASTNode> Parser::parseVariableDeclFromTokens(const Token& typeToken, const Token& nameToken) {
    std::cout << "[DEBUG] :: VariableDeclFromTokens" << std::endl;
    std::string typeName = typeToken.text();
    std::unique_ptr<ASTNode> typeNode = std::make_unique<Identifier>(typeName);
    // Handle pointer/reference tokens between type and variable name
    while (current.type() == TokenType::STAR || current.type() == TokenType::AMPERSAND) {
        if (current.type() == TokenType::STAR) {
            typeNode = std::make_unique<PointerType>(std::move(typeNode));
        } else if (current.type() == TokenType::AMPERSAND) {
            typeNode = std::make_unique<ReferenceType>(std::move(typeNode));
        }
        advance();
    }
    std::string varName = nameToken.text();
    auto varNode = std::make_unique<VarDecl>(varName);
    varNode->type = std::move(typeNode);
    if (match(TokenType::EQUAL)) {
        varNode->initializer = parseExpression();
    }
    expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return varNode;
}

// --- Example: Function Declaration ---
std::unique_ptr<ASTNode> Parser::parseFunctionDecl() {
    std::cout << "[DEBUG] :: FunctionDecl" << std::endl;
    std::string returnType = current.text();
    advance();
    expect(TokenType::IDENTIFIER, "Expected function name");
    std::string funcName = previous().text();
    expect(TokenType::LEFT_PAREN, "Expected '(' after function name");
    auto funcNode = std::make_unique<FunctionDecl>(funcName);
    funcNode->returnType = std::make_unique<Identifier>(returnType);
    // Parse parameters (support multiple)
    std::vector<std::unique_ptr<VarDecl>> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            auto paramType = parseType();
            expect(TokenType::IDENTIFIER, "Expected parameter name");
            std::string paramName = previous().text();
            parameters.push_back(std::make_unique<VarDecl>(paramName, std::move(paramType)));
        } while (match(TokenType::COMMA));
    }
    expect(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    // Convert parameters to ASTNode pointers for FunctionDecl
    std::vector<std::unique_ptr<ASTNode>> paramNodes;
    for (auto& p : parameters) paramNodes.push_back(std::move(p));
    funcNode->parameters = std::move(paramNodes);
    funcNode->body = parseBlock();
    return funcNode;
}
// Helper for parseDeclaration only
std::unique_ptr<ASTNode> Parser::parseFunctionDeclFromTokens(const Token& typeToken, const Token& nameToken) {
    std::cout << "[DEBUG] ::FunctionDeclFromTokens" << std::endl;
    std::string returnType = typeToken.text();
    std::string funcName = nameToken.text();
    expect(TokenType::LEFT_PAREN, "Expected '(' after function name");
    auto funcNode = std::make_unique<FunctionDecl>(funcName);
    funcNode->returnType = std::make_unique<Identifier>(returnType);
    // Parse parameters (support multiple)
    std::vector<std::unique_ptr<VarDecl>> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            auto paramType = parseType();
            expect(TokenType::IDENTIFIER, "Expected parameter name");
            std::string paramName = previous().text();
            parameters.push_back(std::make_unique<VarDecl>(paramName, std::move(paramType)));
        } while (match(TokenType::COMMA));
    }
    expect(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    if (funcNode->isVirtual) {
        std::cout << funcNode->isVirtual <<std::endl;
        if (check(TokenType::EQUAL)) {
            advance();
            if (check(TokenType::INTEGER) && current.text() == "0") {
                advance();
                funcNode->isVirtual = true; // Optionally set a flag for pure virtual
            } else {
                throw std::runtime_error("Expected '0' after '=' for pure virtual function");
            }
        }
    }
    // Convert parameters to ASTNode pointers for FunctionDecl
    std::vector<std::unique_ptr<ASTNode>> paramNodes;
    for (auto& p : parameters) paramNodes.push_back(std::move(p));
    funcNode->parameters = std::move(paramNodes);
    
    if (check(TokenType::LEFT_BRACE)) {
        funcNode->body = parseBlock();
    } else if (check(TokenType::SEMICOLON)) {
        funcNode->body = nullptr;
        advance(); // consume the semicolon
    } else {
        throw std::runtime_error("Expected '{' or ';' after function declaration");
    }
    return funcNode;
}

// --- Example: Block ---
std::unique_ptr<ASTNode> Parser::parseBlock() {
    std::cout << "[DEBUG] :: Block" << std::endl;
    expect(TokenType::LEFT_BRACE, "Expected '{' to start block");
    auto block = std::make_unique<BlockStmt>();
    while (current.type() != TokenType::RIGHT_BRACE && current.type() != TokenType::END_OF_FILE) {
        block->statements.push_back(parseStatement());
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' to end block");
    return block;
}

// --- Example: Statement ---
std::unique_ptr<ASTNode> Parser::parseStatement() {
    std::cout << "[DEBUG] :: Statement" << std::endl;
    if (check(TokenType::IF)) return parseIfStmt();
    if (check(TokenType::WHILE)) return parseWhileStmt();
    if (check(TokenType::FOR)) return parseForStmt();
    if (check(TokenType::SWITCH)) return parseSwitchStmt();
    if (check(TokenType::RETURN)) return parseReturnStmt();
    if (check(TokenType::CONTINUE)) return parseContinueStmt();
    if (check(TokenType::BREAK)) return parseBreakStmt();
    if (current.type() == TokenType::LEFT_BRACE) return parseBlock();
    // Allow variable declarations inside blocks, but only if not assignment
    if (isTypeToken(current.type())) {
        // Peek ahead: type IDENTIFIER [EQUAL|SEMICOLON|LEFT_BRACKET|COMMA]
        Token typeToken = current;
        size_t saveIndex = currentIndex;
        advance();
        if (current.type() == TokenType::IDENTIFIER) {
            Token nameToken = current;
            advance();
            // Only treat as declaration if next token is SEMICOLON, LEFT_BRACKET, or COMMA
            // If next token is EQUAL, treat as declaration ONLY if previous token was not already a variable (i.e., not an assignment)
            if (current.type() == TokenType::SEMICOLON || current.type() == TokenType::LEFT_BRACKET || current.type() == TokenType::COMMA) {
                // Rewind to type token and parse as variable declaration
                currentIndex = saveIndex;
                current = typeToken;
                return parseVariableDecl();
            }
            // If next token is EQUAL, check if this is a declaration or assignment
            if (current.type() == TokenType::EQUAL) {
                // If the type token is a built-in type or known type, treat as declaration
                if (typeToken.type() != TokenType::IDENTIFIER || isTypeToken(typeToken.type())) {
                    currentIndex = saveIndex;
                    current = typeToken;
                    return parseVariableDecl();
                }
                // Otherwise, treat as expression (assignment)
            }
        }
        // Not a declaration, rewind and parse as expression
        currentIndex = saveIndex;
        current = *tokens[currentIndex];
    }
    // Fallback: expression statement
    // size_t exprStart = currentIndex;
    // auto expr = parseExpression();
    // expect(TokenType::SEMICOLON, "Expected ';' after expression");
    // size_t exprEnd = currentIndex;
    // std::string cppExprStr;
    // for (size_t i = exprStart; i < exprEnd; ++i) {
    //     cppExprStr += tokens[i]->text() ;
    // }
    // auto stmt = std::make_unique<ExpressionStmt>(std::move(expr));
    // stmt->cppExpr = cppExprStr;
    // std::cout << stmt->cppExpr<< std::endl;
    // return stmt;
    return parseExpressionstmt();
}

std::unique_ptr<ASTNode> Parser::parseExpressionstmt(){
    std::cout << "[DEBUG] :: ExpressionStmt" << std::endl;
    size_t exprStart = currentIndex;
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after expression");
    size_t exprEnd = currentIndex;
    std::string cppExprStr;
    for (size_t i = exprStart; i < exprEnd; ++i) {
        cppExprStr += tokens[i]->text() ;
    }
    auto stmt = std::make_unique<ExpressionStmt>(std::move(expr));
    stmt->cppExpr = cppExprStr;
    std::cout << stmt->cppExpr<< std::endl;
    return stmt;
}
// --- parseTryStmt ---
std::unique_ptr<ASTNode> Parser::parseTryStmt() {
    std::cout << "[DEBUG] :: TryStmt" << std::endl;
    expect(TokenType::TRY, "Expected 'try'");
    auto tryBlockNode = parseBlock();
    auto tryBlock = std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>(tryBlockNode.release()));
    std::vector<std::unique_ptr<CatchStmt>> catches;
    while (check(TokenType::CATCH)) {
        catches.push_back(parseCatchStmt());
    }
    return std::make_unique<TryStmt>(std::move(tryBlock), std::move(catches));
}

// --- parseThrowStmt ---
std::unique_ptr<ASTNode> Parser::parseThrowStmt() {
    std::cout << "[DEBUG] :: ThrowStmt" << std::endl;
    expect(TokenType::THROW, "Expected 'throw'");
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after throw statement");
    return std::make_unique<ThrowStmt>(std::move(expr));
}

// --- parseBreakStmt ---
std::unique_ptr<ASTNode> Parser::parseBreakStmt() {
    std::cout << "[DEBUG] :: BreakStmt" << std::endl;
    expect(TokenType::BREAK, "Expected 'break'");
    expect(TokenType::SEMICOLON, "Expected ';' after break");
    return std::make_unique<BreakStmt>();
}

// --- parseContinueStmt ---
std::unique_ptr<ASTNode> Parser::parseContinueStmt() {
    std::cout << "[DEBUG] :: ContinueStmt" << std::endl;
    expect(TokenType::CONTINUE, "Expected 'continue'");
    expect(TokenType::SEMICOLON, "Expected ';' after continue");
    return std::make_unique<ContinueStmt>();
}

// --- parseGotoStmt ---
std::unique_ptr<ASTNode> Parser::parseGotoStmt() {
    std::cout << "[DEBUG] :: GotoStmt" << std::endl;
    expect(TokenType::GOTO, "Expected 'goto'");
    expect(TokenType::IDENTIFIER, "Expected label after 'goto'");
    std::string name = previous().text();
    expect(TokenType::SEMICOLON, "Expected ';' after goto statement");
    return std::make_unique<GotoStmt>(name); 
}

// --- parseElseStmt ---
std::unique_ptr<ASTNode> Parser::parseElseStmt() {
    std::cout << "[DEBUG] :: ElseStmt" << std::endl;
    expect(TokenType::ELSE, "Expected 'else'");
    auto elseBranch = parseStatement();
    return std::make_unique<ElseStmt>(std::move(elseBranch));
}

// --- parseSwitchStmt ---
std::unique_ptr<ASTNode> Parser::parseSwitchStmt() {
    std::cout << "[DEBUG] :: SwitchStmt" << std::endl;
    expect(TokenType::SWITCH, "Expected 'switch'");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'switch'");
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after switch condition");
    expect(TokenType::LEFT_BRACE, "Expected '{' after switch");
    std::vector<std::unique_ptr<ASTNode>> cases;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::CASE)) {
            cases.push_back(parseCaseStmt());
        } else if (check(TokenType::DEFAULT)) {
            cases.push_back(parseDefaultStmt());
        } else {
            cases.push_back(parseStatement());
        }
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' after switch body");
    return std::make_unique<SwitchStmt>(std::move(condition), std::move(cases));
}

// --- parseCaseStmt ---
std::unique_ptr<ASTNode> Parser::parseCaseStmt() {
    std::cout << "[DEBUG] :: CaseStmt" << std::endl;
    expect(TokenType::CASE, "Expected 'case'");
    auto value = parseExpression();
    expect(TokenType::COLON, "Expected ':' after case value");
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (!check(TokenType::CASE) && !check(TokenType::DEFAULT) && !check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return std::make_unique<CaseStmt>(std::move(value), std::move(statements));
}

// --- parseDefaultStmt ---
std::unique_ptr<ASTNode> Parser::parseDefaultStmt() {
    std::cout << "[DEBUG] :: DefaultStmt" << std::endl;
    expect(TokenType::DEFAULT, "Expected 'default'");
    expect(TokenType::COLON, "Expected ':' after default");
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (!check(TokenType::CASE) && !check(TokenType::DEFAULT) && !check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return std::make_unique<DefaultStmt>(std::move(statements));
}

// --- parseDoWhileStmt ---
std::unique_ptr<ASTNode> Parser::parseDoWhileStmt() {
    std::cout << "[DEBUG] :: DoWhileStmt" << std::endl;
    expect(TokenType::DO, "Expected 'do'");
    auto body = parseStatement();
    expect(TokenType::WHILE, "Expected 'while' after do body");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after do-while condition");
    expect(TokenType::SEMICOLON, "Expected ';' after do-while");
    return std::make_unique<DoWhileStmt>(std::move(condition), std::move(body));
}

// --- parseUnionDecl ---
std::unique_ptr<ASTNode> Parser::parseUnionDecl() {
    std::cout << "[DEBUG] :: UnionDecl" << std::endl;
    expect(TokenType::UNION, "Expected 'union'");
    expect(TokenType::IDENTIFIER, "Expected union name");
    std::string name = previous().text();

    expect(TokenType::LEFT_BRACE, "Expected '{' after union name");
    std::vector<std::unique_ptr<ASTNode>> members;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        members.push_back(parseVariableDecl());
        expect(TokenType::SEMICOLON, "Expected ';' after union member");
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' after union body");
    expect(TokenType::SEMICOLON, "Expected ';' after union declaration");
   return std::make_unique<UnionDecl>(name, std::move(members));
}

// --- parseTypedefDecl ---
std::unique_ptr<ASTNode> Parser::parseTypedefDecl() {
    std::cout << "[DEBUG] :: TypedefDecl" << std::endl;
    expect(TokenType::TYPEDEF, "Expected 'typedef'");
    auto aliasedType = parseType(); 
    expect(TokenType::IDENTIFIER, "Expected typedef alias name");
    std::string name = previous().text();
    expect(TokenType::SEMICOLON, "Expected ';' after typedef");
    return std::make_unique<TypedefDecl>(name, std::move(aliasedType));
}

// --- parseTemplateTypeSuffix ---
std::unique_ptr<ASTNode> Parser::parseTemplateTypeSuffix(std::string baseName) {
    std::cout << "[DEBUG] :: TemplateTypeSuffix" << std::endl;
    expect(TokenType::LESS, "Expected '<' for template type");
    std::vector<std::unique_ptr<ASTNode>> typeArgs;
    do {
        typeArgs.push_back(parseType());
    } while (match(TokenType::COMMA));
    expect(TokenType::GREATER, "Expected '>' after template arguments");
    return std::make_unique<TemplateType>(baseName, std::move(typeArgs));
}

// --- parseFunctionCallSuffix ---
std::unique_ptr<ASTNode> Parser::parseFunctionCallSuffix(std::unique_ptr<ASTNode> callee) {
    std::cout << "[DEBUG] :: FunctionCallSuffix" << std::endl;
    std::vector<std::unique_ptr<ASTNode>> templateArgs;
    // Check for template instantiation: foo<int>(...)
    if (check(TokenType::LESS)) {
        advance(); // consume '<'
        do {
            templateArgs.push_back(parseType());
        } while (match(TokenType::COMMA));
        expect(TokenType::GREATER, "Expected '>' after template arguments");
    }
    expect(TokenType::LEFT_PAREN, "Expected '(' after function name");
    std::vector<std::unique_ptr<ASTNode>> args;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            args.push_back(parseExpression());
        } while (match(TokenType::COMMA));
    }
    expect(TokenType::RIGHT_PAREN, "Expected ')' after arguments");

    // --- Recognize math and STL algorithm calls ---
    std::string calleeName;
    if (callee->type == ASTNodeType::IDENTIFIER) {
        calleeName = static_cast<Identifier*>(callee.get())->name;
    }
    // Math functions
    if (calleeName == "sqrt" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("sqrt", std::move(args));
    }
    if (calleeName == "pow" && args.size() == 2) {
        return std::make_unique<MathFunctionCall>("pow", std::move(args));
    }
    if (calleeName == "abs" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("abs", std::move(args));
    }
    if (calleeName == "fabs" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("fabs", std::move(args));
    }
    if (calleeName == "sin" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("sin", std::move(args));
    }
    if (calleeName == "cos" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("cos", std::move(args));
    }
    if (calleeName == "tan" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("tan", std::move(args));
    }
    if (calleeName == "floor" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("floor", std::move(args));
    }
    if (calleeName == "ceil" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("ceil", std::move(args));
    }
    if (calleeName == "round" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("round", std::move(args));
    }
    if (calleeName == "rand" && args.size() == 0) {
        return std::make_unique<MathFunctionCall>("rand", std::move(args));
    }
    if (calleeName == "srand" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("srand", std::move(args));
    }
    // String/char functions
    if (calleeName == "strcmp" && args.size() == 2) {
        return std::make_unique<MathFunctionCall>("strcmp", std::move(args));
    }
    if (calleeName == "strncmp" && args.size() == 3) {
        return std::make_unique<MathFunctionCall>("strncmp", std::move(args));
    }
    if (calleeName == "strcpy" && args.size() == 2) {
        return std::make_unique<MathFunctionCall>("strcpy", std::move(args));
    }
    if (calleeName == "strncpy" && args.size() == 3) {
        return std::make_unique<MathFunctionCall>("strncpy", std::move(args));
    }
    if (calleeName == "strlen" && args.size() == 1) {
        return std::make_unique<MathFunctionCall>("strlen", std::move(args));
    }
    if (calleeName == "strcat" && args.size() == 2) {
        return std::make_unique<MathFunctionCall>("strcat", std::move(args));
    }
    if (calleeName == "strncat" && args.size() == 3) {
        return std::make_unique<MathFunctionCall>("strncat", std::move(args));
    }
    // STL algorithms
    if (calleeName == "sort") {
        return std::make_unique<SortCall>(std::move(args));
    }
    if (calleeName == "find") {
        return std::make_unique<FindCall>(std::move(args));
    }
    if (calleeName == "accumulate") {
        return std::make_unique<AccumulateCall>(std::move(args));
    }
    // Fallback: generic function call
    return std::make_unique<FunctionCall>(std::move(callee), std::move(args), std::move(templateArgs));
}


// --- Expression Parsing with Precedence ---
std::unique_ptr<ASTNode> Parser::parseExpression() {
    std::cout << "[DEBUG] :: Expression" << std::endl;
    return parseAssignment();
}

// --- Assignment Expression Parsing ---
std::unique_ptr<ASTNode> Parser::parseAssignment() {
    std::cout << "[DEBUG] :: Assignment" << std::endl;
    auto left = parseTernary();
    // Only allow assignment to identifiers, member access, or array access
    if (match(TokenType::EQUAL)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpr>(std::move(left), std::move(right), "=");
    }
    if (match(TokenType::PLUS_EQUAL)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpr>(std::move(left), std::move(right), "+=");
    }
    if (match(TokenType::MINUS_EQUAL)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpr>(std::move(left), std::move(right), "-=");
    }
    if (match(TokenType::STAR_EQUAL)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpr>(std::move(left), std::move(right), "*=");
    }
    if (match(TokenType::SLASH_EQUAL)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpr>(std::move(left), std::move(right), "/=");
    }
    if (match(TokenType::PERCENT_EQUAL)) {
        auto right = parseAssignment();
        return std::make_unique<AssignmentExpr>(std::move(left), std::move(right), "%=");
    }
    return left;
}


std::unique_ptr<ASTNode> parseBinary(){
    //implement

}
std::unique_ptr<ASTNode> Parser::parseTernary() {
    std::cout << "[DEBUG] :: Ternary" << std::endl;
    auto cond = parseLogicalOr();
    if (match(TokenType::QUESTION)) {
        auto thenExpr = parseExpression();
        expect(TokenType::COLON, "Expected ':' in ternary expression");
        auto elseExpr = parseExpression();
        return std::make_unique<TernaryExpr>(std::move(cond), std::move(thenExpr), std::move(elseExpr));
    }
    return cond;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr() {
    std::cout << "[DEBUG] :: LogicalOr" << std::endl;
    auto left = parseLogicalAnd();
    while (match(TokenType::OR_OR)) {
        auto right = parseLogicalAnd();
        // Defensive: Only construct BinaryExpr if both left and right are valid
        if (!left || !right) {
            // parseLogs.push_back("[WARNING] Invalid BinaryExpr (||) with null child");
            logFile << "[WARNING] Invalid BinaryExpr (||) with null child" << std::endl;
            logFile.flush();
            if (left) return left;
            if (right) return right;
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>("||", std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    std::cout << "[DEBUG] :: LogicalAnd" << std::endl;
    auto left = parseEquality();
    while (match(TokenType::AND_AND)) {
        auto right = parseEquality();
        // Defensive: Only construct BinaryExpr if both left and right are valid
        if (!left || !right) {
            // parseLogs.push_back("[WARNING] Invalid BinaryExpr (&&) with null child");
            logFile << "[WARNING] Invalid BinaryExpr (&&) with null child" << std::endl;
            logFile.flush();
            if (left) return left;
            if (right) return right;
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>("&&", std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    std::cout << "[DEBUG] :: Equality" << std::endl;
    auto left = parseRelational();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL)) {
        std::string op = previous().text();
        auto right = parseRelational();
        // Defensive: Only construct BinaryExpr if both left and right are valid
        if (!left || !right) {
            // parseLogs.push_back("[WARNING] Invalid BinaryExpr (" + op + ") with null child");
            logFile << "[WARNING] Invalid BinaryExpr (" + op + ") with null child" << std::endl;
            logFile.flush();
            if (left) return left;
            if (right) return right;
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseRelational() {
    std::cout << "[DEBUG] :: Relational" << std::endl;
    auto left = parseAdditive();
    while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL)) {
        std::string op = previous().text();
        auto right = parseAdditive();
        // Defensive: Only construct BinaryExpr if both left and right are valid
        if (!left || !right) {
            // parseLogs.push_back("[WARNING] Invalid BinaryExpr (" + op + ") with null child");
            logFile << "[WARNING] Invalid BinaryExpr (" + op + ") with null child" << std::endl;
            logFile.flush();
            if (left) return left;
            if (right) return right;
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    std::cout << "[DEBUG] :: Additive" << std::endl;
    auto left = parseMultiplicative();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string op = previous().text();
        auto right = parseMultiplicative();
        // Defensive: Only construct BinaryExpr if both left and right are valid
        if (!left || !right) {
            // parseLogs.push_back("[WARNING] Invalid BinaryExpr (" + op + ") with null child");
            logFile << "[WARNING] Invalid BinaryExpr (" + op + ") with null child" << std::endl;
            logFile.flush();
            if (left) return left;
            if (right) return right;
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicative() {
    std::cout << "[DEBUG] :: Multiplicative" << std::endl;
    auto left = parseUnary();
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        std::string op = previous().text();
        auto right = parseUnary();
        // Defensive: Only construct BinaryExpr if both left and right are valid
        if (!left || !right) {
            // parseLogs.push_back("[WARNING] Invalid BinaryExpr (" + op + ") with null child");
            logFile << "[WARNING] Invalid BinaryExpr (" + op + ") with null child" << std::endl;
            logFile.flush();
            if (left) return left;
            if (right) return right;
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    std::cout << "[DEBUG] :: Unary" << std::endl;
    if (match(TokenType::EXCLAIM) || match(TokenType::MINUS) || match(TokenType::INCREMENT) || match(TokenType::DECREMENT)) {
        std::string op = previous().text();
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return parsePostfix();
}

std::unique_ptr<ASTNode> Parser::parsePostfix() {
    std::cout << "[DEBUG] :: Postfix" << std::endl;
    auto expr = parsePrimary();
    while (true) {
        if (check(TokenType::LEFT_PAREN)) {
            expr = parseFunctionCallSuffix(std::move(expr));
        } else if (match(TokenType::LEFT_BRACKET)) {
            auto index = parseExpression();
            expect(TokenType::RIGHT_BRACKET, "Expected ']' after array index");
            expr = std::make_unique<ArrayAccess>(std::move(expr), std::move(index));
        } else if (match(TokenType::DOT) || match(TokenType::ARROW)) {
            std::string memberOp = previous().text();
            expect(TokenType::IDENTIFIER, "Expected member name after '.' or '->'");
            std::string member = previous().text();
            expr = std::make_unique<MemberAccess>(std::move(expr), member, memberOp == "->");
        } else if (match(TokenType::SCOPE)) {
            expect(TokenType::IDENTIFIER, "Expected identifier after '::'");
            std::string name = previous().text();
            expr = std::make_unique<QualifiedName>(std::move(expr), name);
        } else if (match(TokenType::INCREMENT)) {
            // Postfix increment: j++
            expr = std::make_unique<UnaryExpr>("++", std::move(expr), true);
        } else if (match(TokenType::DECREMENT)) {
            // Postfix decrement: j--
            expr = std::make_unique<UnaryExpr>("--", std::move(expr), true);
        } else {
            break;
        }
    }
    return expr;
}


std::unique_ptr<ASTNode> Parser::parsePrimary() {
    std::cout << "[DEBUG] :: Primary" << std::endl;
    // C++ casts
    if (match(TokenType::STATIC_CAST)) {
        expect(TokenType::LESS, "Expected '<' after static_cast");
        auto type = parseType();
        expect(TokenType::GREATER, "Expected '>' after type");
        expect(TokenType::LEFT_PAREN, "Expected '(' after '>'");
        auto expr = parseExpression();
        expect(TokenType::RIGHT_PAREN, "Expected ')'");
        return std::make_unique<StaticCastExpr>(std::move(type), std::move(expr));
    }
    if (match(TokenType::DYNAMIC_CAST)) {
        expect(TokenType::LESS, "Expected '<' after dynamic_cast");
        auto type = parseType();
        expect(TokenType::GREATER, "Expected '>' after type");
        expect(TokenType::LEFT_PAREN, "Expected '(' after '>'");
        auto expr = parseExpression();
        expect(TokenType::RIGHT_PAREN, "Expected ')'");
        return std::make_unique<DynamicCastExpr>(std::move(type), std::move(expr));
    }
    if (match(TokenType::CONST_CAST)) {
        expect(TokenType::LESS, "Expected '<' after const_cast");
        auto type = parseType();
        expect(TokenType::GREATER, "Expected '>' after type");
        expect(TokenType::LEFT_PAREN, "Expected '(' after '>'");
        auto expr = parseExpression();
        expect(TokenType::RIGHT_PAREN, "Expected ')'");
        return std::make_unique<ConstCastExpr>(std::move(type), std::move(expr));
    }
    if (match(TokenType::REINTERPRET_CAST)) {
        expect(TokenType::LESS, "Expected '<' after reinterpret_cast");
        auto type = parseType();
        expect(TokenType::GREATER, "Expected '>' after type");
        expect(TokenType::LEFT_PAREN, "Expected '(' after '>'");
        auto expr = parseExpression();
        expect(TokenType::RIGHT_PAREN, "Expected ')'");
        return std::make_unique<ReinterpretCastExpr>(std::move(type), std::move(expr));
    }

    // new/delete
    if (match(TokenType::NEW)) {
        auto type = parseType();
        std::vector<std::unique_ptr<ASTNode>> args;
        if (match(TokenType::LEFT_PAREN)) {
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            expect(TokenType::RIGHT_PAREN, "Expected ')' after new arguments");
        }
        return std::make_unique<NewExpr>(std::move(type), std::move(args));
    }
    if (match(TokenType::DELETE)) {
        bool isArrayDelete = false;
        if (match(TokenType::LEFT_BRACKET)) {
            expect(TokenType::RIGHT_BRACKET, "Expected ']' after '[' in delete[]");
            isArrayDelete = true;
        }
        auto expr = parseExpression();
        return std::make_unique<DeleteExpr>(std::move(expr), isArrayDelete);
    }

    // Lambda
    if (match(TokenType::LEFT_BRACKET)) {
        // Parse capture list (skip for now)
        while (!match(TokenType::RIGHT_BRACKET)) advance();
        expect(TokenType::LEFT_PAREN, "Expected '(' after lambda capture");
        std::vector<std::unique_ptr<ASTNode>> params;
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                auto type = parseType();
                expect(TokenType::IDENTIFIER, "Expected parameter name");
                std::string name = previous().text();
                params.push_back(std::make_unique<VarDecl>(name, std::move(type)));
            } while (match(TokenType::COMMA));
        }
        expect(TokenType::RIGHT_PAREN, "Expected ')' after lambda params");
        auto body = parseBlock();
        return std::make_unique<LambdaExpr>(std::move(params), std::move(body));
    }

    // Literals
    if (match(TokenType::INTEGER)) {
        return std::make_unique<Literal>(previous().text(), "int");
    }
    if (match(TokenType::FLOAT)) {
        return std::make_unique<Literal>(previous().text(), "float");
    }
    if (match(TokenType::STRING_LITERAL)) {
        return std::make_unique<Literal>(previous().text(), "string");
    }
    if (match(TokenType::CHARACTER)) {
        return std::make_unique<Literal>(previous().text(), "char");
    }
    // --- I/O Streams ---
    if (match(TokenType::COUT)) {
        return std::make_unique<CoutExpr>(std::vector<std::unique_ptr<ASTNode>>{});
    }
    if (match(TokenType::CERR)) {
        return std::make_unique<CerrExpr>(std::vector<std::unique_ptr<ASTNode>>{});
    }
    if (match(TokenType::CIN)) {
        return std::make_unique<CinExpr>(std::vector<std::unique_ptr<ASTNode>>{});
    }
    // --- Initializer List ---
    if (match(TokenType::LEFT_BRACE)) {
        std::vector<std::unique_ptr<ASTNode>> elements;
        if (!check(TokenType::RIGHT_BRACE)) {
            do {
                elements.push_back(parseExpression());
            } while (match(TokenType::COMMA));
        }
        expect(TokenType::RIGHT_BRACE, "Expected '}' after initializer list");
        return std::make_unique<InitializerListExpr>(std::move(elements));
    }
    // Identifier
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<Identifier>(previous().text());
    }

    // Parenthesized expression
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RIGHT_PAREN, "Expected ')'");
        return expr;
    }

    throw std::runtime_error("Unexpected token in expression");
}

// --- parseCatchStmt ---
std::unique_ptr<CatchStmt> Parser::parseCatchStmt() {
    std::cout << "[DEBUG] :: CatchStmt" << std::endl;
    expect(TokenType::CATCH, "Expected 'catch'");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'catch'");
    auto exceptionType = parseType();
    expect(TokenType::IDENTIFIER, "Expected exception variable name");
    std::string name = previous().text();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after catch parameter");
    auto bodyNode = parseBlock();
    auto body = std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>(bodyNode.release()));
    return std::make_unique<CatchStmt>(std::move(exceptionType), name, std::move(body));
}


std::unique_ptr<ASTNode> Parser::parseType() {
    std::cout << "[DEBUG] :: Type" << std::endl;
    // Accept all valid type tokens, not just IDENTIFIER
    if (!isTypeToken(current.type())) {
        throw std::runtime_error("Expected type name (at line " + std::to_string(current.line()) + ", column " + std::to_string(current.column()) + ", token: '" + current.text() + "')");
    }
    std::string base = current.text();
    advance();
    std::unique_ptr<ASTNode> typeNode = std::make_unique<Identifier>(base);
    // Built-in types: int, float, double, char, bool, void
    switch (prev.type()) {
        case TokenType::INT:
        case TokenType::FLOAT_TYPE:
        case TokenType::DOUBLE:
        case TokenType::CHAR:
        case TokenType::BOOL:
        case TokenType::VOID:
            // Use QualifiedType for built-in types
            typeNode = std::make_unique<QualifiedType>(base);
            break;
        case TokenType::IDENTIFIER:
            typeNode = std::make_unique<Identifier>(base);
            break;
        default:
            typeNode = std::make_unique<Identifier>(base);
            break;
    }
    // Handle qualified types: A::B::C
    while (match(TokenType::SCOPE)) {
        expect(TokenType::IDENTIFIER, "Expected identifier after '::' in qualified type");
        std::string right = previous().text();
        typeNode = std::make_unique<QualifiedName>(std::move(typeNode), right);
    }
    // Handle template types: vector<int>
    if (match(TokenType::LESS)) {
        std::vector<std::unique_ptr<ASTNode>> typeArgs;
        do {
            typeArgs.push_back(parseType());
        } while (match(TokenType::COMMA));
        expect(TokenType::GREATER, "Expected '>' after template arguments");
        // Wrap in TemplateType
        if (typeNode->type == ASTNodeType::IDENTIFIER) {
            std::string baseName = static_cast<Identifier*>(typeNode.get())->name;
            typeNode = std::make_unique<TemplateType>(baseName, std::move(typeArgs));
        } else if (typeNode->type == ASTNodeType::QUALIFIED_TYPE) {
            // For qualified template types (e.g., std::vector)
            std::string baseName = "";
            ASTNode* cur = typeNode.get();
            while (cur && cur->type == ASTNodeType::QUALIFIED_TYPE) {
                auto* qn = static_cast<QualifiedName*>(cur);
                baseName = qn->right + (baseName.empty() ? "" : "::" + baseName);
                cur = qn->left.get();
            }
            if (cur && cur->type == ASTNodeType::IDENTIFIER) {
                baseName = static_cast<Identifier*>(cur)->name + (baseName.empty() ? "" : "::" + baseName);
            }
            typeNode = std::make_unique<TemplateType>(baseName, std::move(typeArgs));
        }
    }
    // Handle pointer (*) and reference (&) types
    while (true) {
        if (match(TokenType::STAR)) {
            typeNode = std::make_unique<PointerType>(std::move(typeNode));
        } else if (match(TokenType::AMPERSAND)) {
            typeNode = std::make_unique<ReferenceType>(std::move(typeNode));
        } else {
            break;
        }
    }
    return typeNode;
}


std::unique_ptr<ASTNode> Parser::parsePreprocessorDirective() {
    std::cout << "[DEBUG] :: PreprocessorDirective" << std::endl;
    if (!match(TokenType::HASH)) return nullptr;


    if (match(TokenType::PREPROCESSOR_INCLUDE)) {
        // Support both #include "..." and #include <...>
        std::string header;
        if (check(TokenType::STRING)) {
            expect(TokenType::STRING, "Expected header after #include");
            header = previous().text();
        } else if (check(TokenType::LESS)) {
            // Parse <...> as a header
            expect(TokenType::LESS, "Expected '<' after #include");
            std::string headerText;
            while (!check(TokenType::GREATER) && !isAtEnd()) {
                headerText += current.text();
                advance();
            }
            expect(TokenType::GREATER, "Expected '>' after header name");
            header = "<" + headerText + ">";
        } else {
            throw std::runtime_error("Expected header after #include (either \"...\" or <...>)");
        }
        return std::make_unique<PreprocessorInclude>(header);
    }
    if (match(TokenType::PREPROCESSOR_DEFINE)) {
        expect(TokenType::IDENTIFIER, "Expected macro name after #define");
        std::string macro = previous().text();
        std::string value;
        // Optionally parse the macro value (until end of line)
        if (!check(TokenType::NEWLINE) && !isAtEnd()) {
            value = current.text();
            advance();
        }
        return std::make_unique<PreprocessorDefine>(macro, value);
    }
    if (match(TokenType::PREPROCESSOR_UNDEF)) {
        expect(TokenType::IDENTIFIER, "Expected macro name after #undef");
        std::string macro = previous().text();
        return std::make_unique<PreprocessorUndef>(macro);
    }
    if (match(TokenType::PREPROCESSOR_IFDEF)) {
        expect(TokenType::IDENTIFIER, "Expected macro name after #ifdef");
        std::string macro = previous().text();
        return std::make_unique<PreprocessorIfdef>(macro);
    }
    if (match(TokenType::PREPROCESSOR_IFNDEF)) {
        expect(TokenType::IDENTIFIER, "Expected macro name after #ifndef");
        std::string macro = previous().text();
        return std::make_unique<PreprocessorIfndef>(macro);
    }
    if (match(TokenType::PREPROCESSOR_IF)) {
        // Optionally parse the condition as a string or expression
        std::string condition = current.text();
        advance();
        return std::make_unique<PreprocessorIf>(condition);
    }
    if (match(TokenType::PREPROCESSOR_ELSE)) {
        return std::make_unique<PreprocessorElse>();
    }
    if (match(TokenType::PREPROCESSOR_ELIF)) {
        std::string condition = current.text();
        advance();
        return std::make_unique<PreprocessorElif>(condition);
    }
    if (match(TokenType::PREPROCESSOR_ENDIF)) {
        return std::make_unique<PreprocessorEndif>();
    }
    if (match(TokenType::PREPROCESSOR_PRAGMA)) {
        std::string pragma = current.text();
        advance();
        return std::make_unique<PreprocessorPragma>(pragma);
    }

    // Unknown or unsupported directive
    std::string unknown = current.text();
    std::ostringstream err;
    err << "Unknown or unsupported preprocessor directive: '" << unknown << "' at token '" << current.text() << "' (line " << current.line() << ", col " << current.column() << ")";
    std::cerr << err.str() << std::endl;
    advance();
    return std::make_unique<PreprocessorUnknown>(unknown);
}

// --- parseEnumDecl ---
std::unique_ptr<ASTNode> Parser::parseEnumDecl() {
    std::cout << "[DEBUG] :: EnumDecl" << std::endl;
    expect(TokenType::ENUM, "Expected 'enum'");
    expect(TokenType::IDENTIFIER, "Expected enum name");
    std::string name = previous().text();
    expect(TokenType::LEFT_BRACE, "Expected '{' after enum name");
    auto enumNode = std::make_unique<EnumDecl>(name);
    int value = 0;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        expect(TokenType::IDENTIFIER, "Expected enumerator name");
        std::string enumerator = previous().text();
        int enumValue = value;
        if (match(TokenType::EQUAL)) {
            // Parse explicit value
            auto valNode = parseExpression();
            if (valNode->type == ASTNodeType::LITERAL) {
                enumValue = std::stoi(static_cast<Literal*>(valNode.get())->value);
            } else {
                throw std::runtime_error("Enum value must be an integer literal");
            }
            value = enumValue;
        }
        enumNode->enumerators.emplace_back(enumerator, enumValue);
        value++;
        if (!match(TokenType::COMMA)) break;
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' after enum body");
    if (match(TokenType::SEMICOLON)) {
        // optional
    }
    return enumNode;
}

// --- parseNamespaceDecl ---
std::unique_ptr<ASTNode> Parser::parseNamespaceDecl() {
    std::cout << "[DEBUG] :: NamespaceDecl" << std::endl;
    expect(TokenType::NAMESPACE, "Expected 'namespace'");
    expect(TokenType::IDENTIFIER, "Expected namespace name");
    std::string name = previous().text();
    expect(TokenType::LEFT_BRACE, "Expected '{' after namespace name");
    auto nsNode = std::make_unique<NamespaceDecl>(name);
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        nsNode->declarations.push_back(parseDeclaration());
    }
    expect(TokenType::RIGHT_BRACE, "Expected '}' after namespace body");
    if (match(TokenType::SEMICOLON)) {
        // optional
    }
    return nsNode;
}

// --- parseUsingDirective ---
std::unique_ptr<ASTNode> Parser::parseUsingDirective() {
    std::cout << "[DEBUG] :: UsingDirective" << std::endl;
    expect(TokenType::USING, "Expected 'using' directive");
    if (match(TokenType::NAMESPACE)) {
        expect(TokenType::IDENTIFIER, "Expected namespace name after 'using namespace'");
        std::string ns = previous().text();
        expect(TokenType::SEMICOLON, "Expected ';' after using directive");
        return std::make_unique<UsingDirective>(ns);
    } else {
        throw std::runtime_error("Only 'using namespace' directives are supported");
    }
}

// --- parseIfStmt ---
std::unique_ptr<ASTNode> Parser::parseIfStmt() {
    std::cout << "[DEBUG] :: IfStmt" << std::endl;
    expect(TokenType::IF, "Expected 'if'");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after if condition");
    auto thenBranch = parseStatement();
    std::unique_ptr<ASTNode> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = parseStatement();
    }
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

// --- parseWhileStmt ---
std::unique_ptr<ASTNode> Parser::parseWhileStmt() {
    std::cout << "[DEBUG] :: WhileStmt" << std::endl;
    expect(TokenType::WHILE, "Expected 'while'");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after while condition");
    auto body = parseStatement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

// --- parseForStmt ---
std::unique_ptr<ASTNode> Parser::parseForStmt() {
    std::cout << "[DEBUG] :: ForStmt" << std::endl;
    expect(TokenType::FOR, "Expected 'for'");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    std::unique_ptr<ASTNode> init = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        if (isTypeToken(current.type()) && current.type() != TokenType::IDENTIFIER) {
            // Variable declaration as initializer
            init = parseVariableDecl();
        } else {
            // Expression or assignment as initializer
            init = parseStatement();
            // Only expect semicolon if not already consumed by parseVariableDecl
            // if (current.type() == TokenType::SEMICOLON) advance();
        }
    } else {
        advance(); // skip ;
    }
    std::unique_ptr<ASTNode> cond = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        cond = parseExpression();
    }
    expect(TokenType::SEMICOLON, "Expected ';' after for-condition");
    std::unique_ptr<ASTNode> inc = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        inc = parseExpression();
    }
    expect(TokenType::RIGHT_PAREN, "Expected ')' after for-clause");
    auto body = parseStatement();
    return std::make_unique<ForStmt>(std::move(init), std::move(cond), std::move(inc), std::move(body));
}

// --- parseReturnStmt ---
std::unique_ptr<ASTNode> Parser::parseReturnStmt() {
    std::cout << "[DEBUG] :: ReturnStmt" << std::endl;
    expect(TokenType::RETURN, "Expected 'return'");
    std::unique_ptr<ASTNode> expr = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        expr = parseExpression();
    }
    expect(TokenType::SEMICOLON, "Expected ';' after return statement");
    return std::make_unique<ReturnStmt>(std::move(expr));
}

// --- parseTemplateDecl ---
std::unique_ptr<ASTNode> Parser::parseTemplateDecl() {
    std::cout << "[DEBUG] :: TemplateDecl" << std::endl;
    expect(TokenType::TEMPLATE, "Expected 'template'");
    expect(TokenType::LESS, "Expected '<' after 'template'");
    // Parse template parameter list (e.g., typename T, class U)
    std::vector<std::unique_ptr<TemplateParam>> templateParams;
    std::vector<std::string> paramNames; // For TemplateDecl fallback
    do {
        if ((check(TokenType::CLASS) || current.text() == "typename") &&
            tokens[currentIndex + 1]->type() == TokenType::IDENTIFIER) {
            advance(); // skip 'class' or 'typename'
            expect(TokenType::IDENTIFIER, "Expected template parameter name");
            std::string paramName = previous().text();
            templateParams.push_back(std::make_unique<TemplateParam>(paramName, true));
            paramNames.push_back(paramName);
        } else if (check(TokenType::IDENTIFIER) && tokens[currentIndex + 1]->type() == TokenType::IDENTIFIER) {
            // Accept: template <T U>
            std::string typeName = current.text();
            advance();
            expect(TokenType::IDENTIFIER, "Expected template parameter name");
            std::string paramName = previous().text();
            templateParams.push_back(std::make_unique<TemplateParam>(paramName, false, typeName));
            paramNames.push_back(paramName);
        } else {
            throw std::runtime_error("Expected template parameter declaration (class/typename/identifier identifier)");
        }
    } while (match(TokenType::COMMA));
    expect(TokenType::GREATER, "Expected '>' after template parameter list");

    // --- After parsing template parameter list, handle 'class' or 'struct' or 'enum' or 'union' as identifier tokens
    if (check(TokenType::IDENTIFIER) && (current.text() == "class" || current.text() == "struct" || current.text() == "enum" || current.text() == "union")) {
        TokenType keywordType = TokenType::CLASS;
        if (current.text() == "struct") keywordType = TokenType::STRUCT;
        else if (current.text() == "enum") keywordType = TokenType::ENUM;
        else if (current.text() == "union") keywordType = TokenType::UNION;
        current = Token(keywordType, current.text(), current.line(), current.column());
    }

    if (check(TokenType::CLASS)) {
        // Template class
        auto classNode = parseClassDecl();
        auto* classPtr = dynamic_cast<ClassDecl*>(classNode.release());
        if (!classPtr) throw std::runtime_error("Expected ClassDecl after template<...> class");
        std::string className = classPtr->name;
        std::vector<std::unique_ptr<ASTNode>> members;
        // Collect all members from all access levels
        for (auto& m : classPtr->publicMembers) {
            members.push_back(std::move(m));
        }
        for (auto& m : classPtr->privateMembers) {
            members.push_back(std::move(m));
        }
        for (auto& m : classPtr->protectedMembers) {
            members.push_back(std::move(m));
        }
        delete classPtr; // classPtr is now empty, members moved
        return std::make_unique<TemplateClassDecl>(className, std::move(templateParams), std::move(members));
    } else {
        // Fallback: generic template decl
        auto decl = parseDeclaration();
        // TemplateDecl does not exist; use TemplateClassDecl as a generic wrapper
        std::vector<std::unique_ptr<ASTNode>> nodes;
        nodes.push_back(std::move(decl));
        return std::make_unique<TemplateClassDecl>(
            paramNames.empty() ? "" : paramNames[0],
            std::vector<std::unique_ptr<TemplateParam>>{},
            std::move(nodes)
        );
    }
}

