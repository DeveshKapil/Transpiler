#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include "token.hpp"

enum class ASTNodeType {
    PROGRAM, FUNCTION_DECL, VAR_DECL, IF_STMT, RETURN_STMT, BINARY_EXPR, LITERAL, IDENTIFIER
};

class ASTNode {
public:
    ASTNodeType type;
    explicit ASTNode(ASTNodeType type) : type(type) {}
    virtual ~ASTNode() = default;
    virtual std::string toString(int indent = 0) const = 0;
};

class FunctionDecl : public ASTNode {
public:
    std::string returnType;
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> statements;
    FunctionDecl(std::string retType, std::string name)
        : ASTNode(ASTNodeType::FUNCTION_DECL), returnType(std::move(retType)), name(std::move(name)) {}
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "FunctionDecl(" + returnType + " " + name + ")\n";
        for (const auto& stmt : statements) {
            result += stmt->toString(indent + 2);
        }
        return result;
    }
};

class VarDecl : public ASTNode {
public:
    std::string varType;
    std::string name;
    std::unique_ptr<ASTNode> initializer;
    VarDecl(std::string type, std::string name, std::unique_ptr<ASTNode> init)
        : ASTNode(ASTNodeType::VAR_DECL), varType(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "VarDecl(" + varType + " " + name + " = ";
        result += initializer ? initializer->toString(0) : "null";
        return result + ")\n";
    }
};

class IfStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBranch;
    IfStmt(std::unique_ptr<ASTNode> cond, std::vector<std::unique_ptr<ASTNode>> then)
        : ASTNode(ASTNodeType::IF_STMT), condition(std::move(cond)), thenBranch(std::move(then)) {}
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "IfStmt(condition=" + condition->toString(0) + ")\n";
        for (const auto& stmt : thenBranch) {
            result += stmt->toString(indent + 2);
        }
        return result;
    }
};

class ReturnStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
    explicit ReturnStmt(std::unique_ptr<ASTNode> expr)
        : ASTNode(ASTNodeType::RETURN_STMT), expression(std::move(expr)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ReturnStmt(" + (expression ? expression->toString(0) : "null") + ")\n";
    }
};

class BinaryExpr : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryExpr(std::string op, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : ASTNode(ASTNodeType::BINARY_EXPR), op(std::move(op)), left(std::move(l)), right(std::move(r)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "BinaryExpr(" + left->toString(0) + " " + op + " " + right->toString(0) + ")\n";
    }
};

class Literal : public ASTNode {
public:
    std::string value;
    std::string type;
    Literal(std::string val, std::string t)
        : ASTNode(ASTNodeType::LITERAL), value(std::move(val)), type(std::move(t)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "Literal(" + type + " " + value + ")";
    }
};

class Identifier : public ASTNode {
public:
    std::string name;
    explicit Identifier(std::string n)
        : ASTNode(ASTNodeType::IDENTIFIER), name(std::move(n)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "Identifier(" + name + ")";
    }
};
#endif