#ifndef JAVA_CODE_GENERATOR_HPP
#define JAVA_CODE_GENERATOR_HPP

#include "ast.hpp"
#include <string>

class JavaCodeGenerator {
public:
    std::string generate(const ASTNode* node , const std::string outputfile) const;
private:
    std::string generateFunctionDecl(const FunctionDecl* node , const std::string outputfile) const;
    std::string generateVarDecl(const VarDecl* node) const;
    std::string generateIfStmt(const IfStmt* node) const;
    std::string generateReturnStmt(const ReturnStmt* node) const;
    std::string generateBinaryExpr(const BinaryExpr* node) const;
    std::string generateLiteral(const Literal* node) const;
    std::string generateIdentifier(const Identifier* node) const;
};

#endif // JAVA_CODE_GENERATOR_HPP