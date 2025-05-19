#include "JavaCodeGenerator.hpp"
#include <sstream>

std::string JavaCodeGenerator::generate(const ASTNode* node, const std::string& className) const {
    switch (node->type) {
        case ASTNodeType::FUNCTION_DECL:
            return generateFunctionDecl(static_cast<const FunctionDecl*>(node), className);
        case ASTNodeType::VAR_DECL:
            return generateVarDecl(static_cast<const VarDecl*>(node), className);
        case ASTNodeType::IF_STMT:
            return generateIfStmt(static_cast<const IfStmt*>(node), className);
        case ASTNodeType::RETURN_STMT:
            return generateReturnStmt(static_cast<const ReturnStmt*>(node), className);
        case ASTNodeType::BINARY_EXPR:
            return generateBinaryExpr(static_cast<const BinaryExpr*>(node), className);
        case ASTNodeType::LITERAL:
            return generateLiteral(static_cast<const Literal*>(node), className);
        case ASTNodeType::IDENTIFIER:
            return generateIdentifier(static_cast<const Identifier*>(node), className);
        default:
            return "";
    }
}

std::string JavaCodeGenerator::generateFunctionDecl(const FunctionDecl* node, const std::string& className) const {
    std::ostringstream oss;
    oss << "public class " << className << " {\n";
    oss << "    public static " << node->returnType << " " << node->name << "() {\n";
    for (const auto& stmt : node->statements) {
        oss << "        " << generate(stmt.get(), className) << "\n";
    }
    oss << "    }\n";
    oss << "}\n";
    return oss.str();
}

std::string JavaCodeGenerator::generateVarDecl(const VarDecl* node, const std::string& className) const {
    std::ostringstream oss;
    oss << node->varType << " " << node->name << " = " << generate(node->initializer.get(), className) << ";";
    return oss.str();
}

std::string JavaCodeGenerator::generateIfStmt(const IfStmt* node, const std::string& className) const {
    std::ostringstream oss;
    oss << "if (" << generate(node->condition.get(), className) << ") {\n";
    for (const auto& stmt : node->thenBranch) {
        oss << "    " << generate(stmt.get(), className) << "\n";
    }
    oss << "}";
    return oss.str();
}

std::string JavaCodeGenerator::generateReturnStmt(const ReturnStmt* node, const std::string& className) const {
    std::ostringstream oss;
    oss << "return " << (node->expression ? generate(node->expression.get(), className) : "") << ";";
    return oss.str();
}

std::string JavaCodeGenerator::generateBinaryExpr(const BinaryExpr* node, const std::string& className) const {
    std::ostringstream oss;
    oss << generate(node->left.get(), className) << " " << node->op << " " << generate(node->right.get(), className);
    return oss.str();
}

std::string JavaCodeGenerator::generateLiteral(const Literal* node, const std::string& className) const {
    return node->value;
}

std::string JavaCodeGenerator::generateIdentifier(const Identifier* node, const std::string& className) const {
    return node->name;
} 