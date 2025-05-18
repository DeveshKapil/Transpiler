#include "JavaCodeGenerator.hpp"
#include <sstream>

std::string JavaCodeGenerator::generate(const ASTNode* node) const {
    switch (node->type) {
        case ASTNodeType::FUNCTION_DECL:
            return generateFunctionDecl(static_cast<const FunctionDecl*>(node));
        case ASTNodeType::VAR_DECL:
            return generateVarDecl(static_cast<const VarDecl*>(node));
        case ASTNodeType::IF_STMT:
            return generateIfStmt(static_cast<const IfStmt*>(node));
        case ASTNodeType::RETURN_STMT:
            return generateReturnStmt(static_cast<const ReturnStmt*>(node));
        case ASTNodeType::BINARY_EXPR:
            return generateBinaryExpr(static_cast<const BinaryExpr*>(node));
        case ASTNodeType::LITERAL:
            return generateLiteral(static_cast<const Literal*>(node));
        case ASTNodeType::IDENTIFIER:
            return generateIdentifier(static_cast<const Identifier*>(node));
        default:
            return "";
    }
}

std::string JavaCodeGenerator::generateFunctionDecl(const FunctionDecl* node) const {
    std::ostringstream oss;
    // For simplicity, wrap everything in a class called Main
    oss << "public class Main {\n";
    oss << "    public static " << node->returnType << " " << node->name << "() {\n";
    for (const auto& stmt : node->statements) {
        oss << "        " << generate(stmt.get()) << "\n";
    }
    oss << "    }\n";
    oss << "}\n";
    return oss.str();
}

std::string JavaCodeGenerator::generateVarDecl(const VarDecl* node) const {
    std::ostringstream oss;
    oss << node->varType << " " << node->name << " = " << generate(node->initializer.get()) << ";";
    return oss.str();
}

std::string JavaCodeGenerator::generateIfStmt(const IfStmt* node) const {
    std::ostringstream oss;
    oss << "if (" << generate(node->condition.get()) << ") {\n";
    for (const auto& stmt : node->thenBranch) {
        oss << "    " << generate(stmt.get()) << "\n";
    }
    oss << "}";
    return oss.str();
}

std::string JavaCodeGenerator::generateReturnStmt(const ReturnStmt* node) const {
    std::ostringstream oss;
    oss << "return " << (node->expression ? generate(node->expression.get()) : "") << ";";
    return oss.str();
}

std::string JavaCodeGenerator::generateBinaryExpr(const BinaryExpr* node) const {
    std::ostringstream oss;
    oss << generate(node->left.get()) << " " << node->op << " " << generate(node->right.get());
    return oss.str();
}

std::string JavaCodeGenerator::generateLiteral(const Literal* node) const {
    return node->value;
}

std::string JavaCodeGenerator::generateIdentifier(const Identifier* node) const {
    return node->name;
} 