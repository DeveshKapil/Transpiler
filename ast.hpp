#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// #include <optional>
#include "tokens.hpp"  // your existing token types for reference if needed

// Enumerate all node types in AST
enum class ASTNodeType {
    PROGRAM,
    PREPROCESSOR_DIRECTIVE,
    NAMESPACE_DECL,
    USING_DIRECTIVE,
    
    CLASS_DECL,
    STRUCT_DECL,
    ENUM_DECL,
    UNION_DECL,
    
    FUNCTION_DECL,
    VAR_DECL,
    TYPEDEF_DECL,
    
    IF_STMT,
    ELSE_STMT,  // if you want explicit else node
    SWITCH_STMT,
    CASE_STMT,
    DEFAULT_STMT,
    FOR_STMT,
    WHILE_STMT,
    DO_WHILE_STMT,
    RETURN_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    GOTO_STMT,
    
    TRY_STMT,
    CATCH_STMT,
    THROW_STMT,
    
    BLOCK_STMT,
    EXPRESSION_STMT,
    ASSIGNMENT_EXPR,
    BINARY_EXPR,
    UNARY_EXPR,
    TERNARY_EXPR,
    
    FUNCTION_CALL,
    MEMBER_ACCESS,
    ARRAY_ACCESS,
    VECTOR_ACCESS,
    
    LITERAL,
    IDENTIFIER,
    

    // Namespace and Template tokens
    TEMPLATE_CLASS_DECL,
    TEMPLATE_TYPE,
    POINTER_TYPE,
    REFERENCE_TYPE,
    QUALIFIED_TYPE,  // e.g., std::vector
    QUALIFIED_NAME,
    TEMPLATE_KEYWORD,
    USING_KEYWORD,
    NAMESPACE_KEYWORD,
    TEMPLATE_LESS,
    TEMPLATE_GREATER,
    TEMPLATE_COMMA,
    TEMPLATE_FUNCTION_DECL,
    // Template parameters and arguments
    TEMPLATE_PARAM,
    TEMPLATE_ARG,
    
    TYPE,  // generic type node
    
    LAMBDA_EXPR,
    
    // Special nodes for casts and RTTI
    STATIC_CAST_EXPR,
    DYNAMIC_CAST_EXPR,
    CONST_CAST_EXPR,
    REINTERPRET_CAST_EXPR,
    TYPEID_EXPR,
    
    // Stream Expressions
    STREAM_EXPR,  // for cout << x;
    
    
    
    // Preprocessor
    PREPROCESSOR_INCLUDE,
    PREPROCESSOR_DEFINE,
    PREPROCESSOR_IFDEF,
    PREPROCESSOR_IFNDEF,
    PREPROCESSOR_IF,
    PREPROCESSOR_ELIF,
    PREPROCESSOR_ELSE,
    PREPROCESSOR_ENDIF,
    PREPROCESSOR_UNDEF,
    PREPROCESSOR_PRAGMA,
    PREPROCESSOR_UNKNOWN,
    
    // Concurrency
    THREAD_DECL,
    MUTEX_DECL,
    LOCK_GUARD_DECL,
    UNIQUE_LOCK_DECL,
    ASYNC_EXPR,
    FUTURE_EXPR,
    PROMISE_DECL,
    
    // Exception Classes
    EXCEPTION_CLASS,
    LOGIC_ERROR_CLASS,
    RUNTIME_ERROR_CLASS,
    
    // STL containers
    VECTOR_TYPE,
    MAP_TYPE,
    SET_TYPE,
    LIST_TYPE,
    DEQUE_TYPE,
    UNORDERED_MAP_TYPE,
    UNORDERED_SET_TYPE,
    MULTIMAP_TYPE,
    MULTISET_TYPE,
    STACK_TYPE,
    QUEUE_TYPE,
    PRIORITY_QUEUE_TYPE,
    BITSET_TYPE,
    ARRAY_TYPE,
    FORWARD_LIST_TYPE,
    PAIR_TYPE,
    TUPLE_TYPE,
    STRING_TYPE,
    OPTIONAL_TYPE,
    VARIANT_TYPE,
    ANY_TYPE,
    SPAN_TYPE,
    VALARRAY_TYPE,
    INITIALIZER_LIST_EXPR,

    // Std functions
    PRINTF_CALL,
    SCANF_CALL,
    MALLOC_CALL,
    FREE_CALL,
    MEMCPY_CALL,
    STRCPY_CALL,
    STRLEN_CALL,
    
    // I/O Streams
    CIN_EXPR,
    COUT_EXPR,
    CERR_EXPR,
    CLIN_EXPR,
    GETLINE_CALL,
    PUT_CALL,
    GET_CALL,
    FLUSH_CALL,
    OPEN_CALL,
    CLOSE_CALL,
    READ_CALL,
    WRITE_CALL,
    
    // Algorithms
    SORT_CALL,
    FIND_CALL,
    COUNT_CALL,
    COPY_CALL,
    REVERSE_CALL,
    ACCUMULATE_CALL,
    ALL_OF_CALL,
    ANY_OF_CALL,
    NONE_OF_CALL,
    LOWER_BOUND_CALL,
    UPPER_BOUND_CALL,
    
    // Math Functions
    ABS_CALL,
    FABS_CALL,
    POW_CALL,
    SQRT_CALL,
    SIN_CALL,
    COS_CALL,
    TAN_CALL,
    FLOOR_CALL,
    CEIL_CALL,
    ROUND_CALL,
    RAND_CALL,
    SRAND_CALL,
    EXIT_CALL,
    
    // String functions
    STOI_CALL,
    STOF_CALL,
    STOD_CALL,
    TO_STRING_CALL,
    STRCMP_CALL,
    STRNCMP_CALL,
    STRCHR_CALL,
    STRRCHR_CALL,
    STRSTR_CALL,
    STRCAT_CALL,
    STRNCAT_CALL,
    
    // Memory
    NEW_EXPR,
    DELETE_EXPR,
    ALLOCATE_CALL,
    DEALLOCATE_CALL,
    
    // Time functions
    TIME_CALL,
    CLOCK_CALL,
    DIFFTIME_CALL,
    STRFTIME_CALL,
    LOCALTIME_CALL,
    GMTIME_CALL,
    
    
    
    // Keywords and other tokens
    CONST_KEYWORD,
    STATIC_KEYWORD,
    EXTERN_KEYWORD,
    REGISTER_KEYWORD,
    INLINE_KEYWORD,
    VIRTUAL_KEYWORD,
    EXPLICIT_KEYWORD,
    FRIEND_KEYWORD,
    PRIVATE_KEYWORD,
    PUBLIC_KEYWORD,
    PROTECTED_KEYWORD,
    
    // Logical & control keywords
    IF_KEYWORD,
    ELSE_KEYWORD,
    FOR_KEYWORD,
    WHILE_KEYWORD,
    DO_KEYWORD,
    SWITCH_KEYWORD,
    CASE_KEYWORD,
    DEFAULT_KEYWORD,
    BREAK_KEYWORD,
    CONTINUE_KEYWORD,
    RETURN_KEYWORD,
    GOTO_KEYWORD,
    
    // Exception keywords
    TRY_KEYWORD,
    CATCH_KEYWORD,
    THROW_KEYWORD,
    
    // Cast keywords
    STATIC_CAST_KEYWORD,
    DYNAMIC_CAST_KEYWORD,
    CONST_CAST_KEYWORD,
    REINTERPRET_CAST_KEYWORD,
    
    // Additional nodes as needed
};


class TemplateType;
class ASTNode;
class Expression;
class Statement;
class Program;
class PreprocessorDirective;
class NamespaceDecl;
class UsingDirective;
class ClassDecl;
class StructDecl;
class EnumDecl;
class UnionDecl;
class FunctionDecl;
class VarDecl;
class TypedefDecl;
class TemplateParam;
class InitializerListExpr;
class BlockStmt;
class ExpressionStmt;
class IfStmt;
class ElseStmt;
class WhileStmt;
class DoWhileStmt;
class ForStmt;
class ReturnStmt;
class BreakStmt;
class ContinueStmt;
class GotoStmt;
class TryStmt;
class CatchStmt;
class ThrowStmt;
class SwitchStmt;
class CaseStmt;
class DefaultStmt;
class Literal;
class Identifier;
class BinaryExpr;
class UnaryExpr;
class TernaryExpr;
class FunctionCall;
class MemberAccess;
class ArrayAccess;
class ArrayType;
class VectorAccess;
class StreamExpr;
class LambdaExpr;
class StaticCastExpr;
class DynamicCastExpr;
class ConstCastExpr;
class ReinterpretCastExpr;
class TypeidExpr;
class TemplateClassDecl;
class TemplateArg;
class TemplateFunctionDecl;
class QualifiedType;
class QualifiedName;
class PointerType;
class ReferenceType;
class ThreadDecl;
class MutexDecl;
class LockGuardDecl;
class AsyncExpr;
class FutureExpr;
class PromiseDecl;
class MathFunctionCall;
class SortCall;
class FindCall;
class AccumulateCall;
class CoutExpr;
class CinExpr;
class CerrExpr;
class GetlineCall;
class PrintfCall;
class ScanfCall;
class NewExpr;
class DeleteExpr;
class MallocCall;
class FreeCall;
class AbsCall;
class PreprocessorInclude;
class PreprocessorDefine;
class PreprocessorUndef;
class PreprocessorIfdef;
class PreprocessorIfndef;
class PreprocessorIf;
class PreprocessorElse;
class PreprocessorElif;
class PreprocessorEndif;
class PreprocessorPragma;
class PreprocessorUnknown;

// Base ASTNode class
class ASTNode {
public:
    ASTNodeType type;
    explicit ASTNode(ASTNodeType t) : type(t) {}
    virtual ~ASTNode() = default;
    virtual std::string toString(int indent = 0) const {
        return std::string(indent, ' ') ;
    }
};


// Program node: root container for all global declarations
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> globals;
    Program() : ASTNode(ASTNodeType::PROGRAM) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "Program\n";
        for (const auto& g : globals) {
            if (g) s += g->toString(indent + 2) + "\n";
        }
        return s;
    }
};

class PreprocessorDirective : public ASTNode {
public:
    std::string directiveText;
    explicit PreprocessorDirective(std::string text)
        : ASTNode(ASTNodeType::PREPROCESSOR_DIRECTIVE), directiveText(std::move(text)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorDirective: " + directiveText;
    }
};

class NamespaceDecl : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> declarations;
    explicit NamespaceDecl(std::string nsName)
        : ASTNode(ASTNodeType::NAMESPACE_DECL), name(std::move(nsName)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "NamespaceDecl: " + name + "\n";
        for (const auto& d : declarations) {
            if (d) s += d->toString(indent + 2) + "\n";
        }
        return s;
    }
};

class UsingDirective : public ASTNode {
public:
    std::string namespaceName;
    explicit UsingDirective(std::string ns)
        : ASTNode(ASTNodeType::USING_DIRECTIVE), namespaceName(std::move(ns)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "UsingDirective: " + namespaceName;
    }
};

struct BaseSpecifier {
    std::string baseName;
    std::string access; // "public", "protected", "private"
    BaseSpecifier(const std::string& n, const std::string& a) : baseName(n), access(a) {}
};

class ClassDecl : public ASTNode {
public:
    std::string name;
    std::vector<BaseSpecifier> bases;
    std::vector<std::unique_ptr<ASTNode>> publicMembers;
    std::vector<std::unique_ptr<ASTNode>> privateMembers;
    std::vector<std::unique_ptr<ASTNode>> protectedMembers;
    explicit ClassDecl(std::string className)
        : ASTNode(ASTNodeType::CLASS_DECL), name(std::move(className)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "ClassDecl: " + name + "\n";
        if (!publicMembers.empty()) {
            s += std::string(indent + 2, ' ') + "Public Members:\n";
            for (const auto& m : publicMembers) if (m) s += m->toString(indent + 4) + "\n";
        }
        if (!privateMembers.empty()) {
            s += std::string(indent + 2, ' ') + "Private Members:\n";
            for (const auto& m : privateMembers) if (m) s += m->toString(indent + 4) + "\n";
        }
        if (!protectedMembers.empty()) {
            s += std::string(indent + 2, ' ') + "Protected Members:\n";
            for (const auto& m : protectedMembers) if (m) s += m->toString(indent + 4) + "\n";
        }
        return s;
    }
};

class StructDecl : public ASTNode {
public:
    std::string name;
    std::vector<BaseSpecifier> bases;
    std::vector<std::unique_ptr<ASTNode>> publicMembers;
    std::vector<std::unique_ptr<ASTNode>> privateMembers;
    std::vector<std::unique_ptr<ASTNode>> protectedMembers;
    explicit StructDecl(std::string structName)
        : ASTNode(ASTNodeType::STRUCT_DECL), name(std::move(structName)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "ClassDecl: " + name + "\n";
        if (!publicMembers.empty()) {
            s += std::string(indent + 2, ' ') + "Public Members:\n";
            for (const auto& m : publicMembers) if (m) s += m->toString(indent + 4) + "\n";
        }
        if (!privateMembers.empty()) {
            s += std::string(indent + 2, ' ') + "Private Members:\n";
            for (const auto& m : privateMembers) if (m) s += m->toString(indent + 4) + "\n";
        }
        if (!protectedMembers.empty()) {
            s += std::string(indent + 2, ' ') + "Protected Members:\n";
            for (const auto& m : protectedMembers) if (m) s += m->toString(indent + 4) + "\n";
        }
        return s;
    }
};

class EnumDecl : public ASTNode {
public:
    std::string name;
    std::vector<std::pair<std::string, int>> enumerators;
    explicit EnumDecl(std::string enumName)
        : ASTNode(ASTNodeType::ENUM_DECL), name(std::move(enumName)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "EnumDecl: " + name + "\n";
        for (const auto& e : enumerators) {
            s += std::string(indent + 2, ' ') + e.first + " = " + std::to_string(e.second) + "\n";
        }
        return s;
    }
};

class UnionDecl : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> members;
    UnionDecl(std::string n, std::vector<std::unique_ptr<ASTNode>> m)
        : ASTNode(ASTNodeType::UNION_DECL), name(std::move(n)), members(std::move(m)) {}
    explicit UnionDecl(std::string unionName)
        : ASTNode(ASTNodeType::UNION_DECL), name(std::move(unionName)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "UnionDecl: " + name + "\n";
        for (const auto& m : members) if (m) s += m->toString(indent + 2) + "\n";
        return s;
    }
};

class FunctionDecl : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> returnType;
    std::vector<std::unique_ptr<ASTNode>> parameters;
    std::unique_ptr<ASTNode> body;
    bool isConst = false;
    bool isVirtual = false;
    bool isStatic = false;
    bool isConstructor = false;
    bool isDestructor = false;
    bool isInline = false;
    bool isFriend = false;
    explicit FunctionDecl(std::string funcName)
        : ASTNode(ASTNodeType::FUNCTION_DECL), name(std::move(funcName)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "FunctionDecl: " + name;
        if (returnType) s += " returns " + returnType->toString(0);
        s += "\n";
        if (!parameters.empty()) {
            s += std::string(indent + 2, ' ') + "Parameters:\n";
            for (const auto& p : parameters) if (p) s += p->toString(indent + 4) + "\n";
        }
        if (body) s += body->toString(indent + 2) + "\n";
        return s;
    }
};

class VarDecl : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> type;
    std::unique_ptr<ASTNode> initializer;
    bool isStatic = false;
    bool isConst = false;
    VarDecl(std::string varName,
            std::unique_ptr<ASTNode> typeNode = nullptr,
            std::unique_ptr<ASTNode> init = nullptr,
            bool isStatic_ = false,
            bool isConst_ = false)
        : ASTNode(ASTNodeType::VAR_DECL),
          name(std::move(varName)),
          type(std::move(typeNode)),
          initializer(std::move(init)),
          isStatic(isStatic_),
          isConst(isConst_) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "VarDecl: " + name;
        if (type) s += " : " + type->toString(0);
        if (initializer) s += " = " + initializer->toString(0);
        return s + "\n";
    }
};

class MultiVarDecl : public ASTNode {
public:
    std::unique_ptr<ASTNode> type;
    std::vector<std::string> names;
    std::vector<std::unique_ptr<ASTNode>> initializers;
    MultiVarDecl(std::unique_ptr<ASTNode> typeNode, std::vector<std::string> n, std::vector<std::unique_ptr<ASTNode>> inits)
        : ASTNode(ASTNodeType::VAR_DECL), type(std::move(typeNode)), names(std::move(n)), initializers(std::move(inits)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "MultiVarDecl: ";
        if (type) s += type->toString(0);
        for (size_t i = 0; i < names.size(); ++i) {
            s += " " + names[i];
            if (initializers[i]) s += " = " + initializers[i]->toString(0);
            if (i + 1 < names.size()) s += ",";
        }
        return s + "\n";
    }
};



class TypedefDecl : public ASTNode {
public:
    std::string aliasName;
    std::unique_ptr<ASTNode> aliasedType;
    TypedefDecl(std::string alias, std::unique_ptr<ASTNode> aliased)
        : ASTNode(ASTNodeType::TYPEDEF_DECL), aliasName(std::move(alias)), aliasedType(std::move(aliased)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "TypedefDecl: " + aliasName;
        if (aliasedType) s += " = " + aliasedType->toString(0);
        return s;
    }
};

class Statement : public ASTNode {
public:
    explicit Statement(ASTNodeType type) : ASTNode(type) {}
};

class BlockStmt : public Statement {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockStmt() : Statement(ASTNodeType::BLOCK_STMT) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "BlockStmt\n";
        for (const auto& stmt : statements) if (stmt) s += stmt->toString(indent + 2) + "\n";
        return s;
    }
};

class ExpressionStmt : public Statement {
public:
    std::unique_ptr<ASTNode> expression;
    std::string cppExpr;
    explicit ExpressionStmt(std::unique_ptr<ASTNode> expr)
        : Statement(ASTNodeType::EXPRESSION_STMT), expression(std::move(expr)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ExpressionStmt: " + (expression ? expression->toString(0) : "null");
    }
};

class IfStmt : public Statement {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch;
    IfStmt()
        : Statement(ASTNodeType::IF_STMT) {}
    IfStmt(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> thenBr, std::unique_ptr<ASTNode> elseBr)
        : Statement(ASTNodeType::IF_STMT), condition(std::move(cond)), thenBranch(std::move(thenBr)), elseBranch(std::move(elseBr)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "IfStmt\n";
        if (condition) s += std::string(indent + 2, ' ') + "Condition: " + condition->toString(0) + "\n";
        if (thenBranch) s += std::string(indent + 2, ' ') + "Then: " + thenBranch->toString(indent + 4) + "\n";
        if (elseBranch) s += std::string(indent + 2, ' ') + "Else: " + elseBranch->toString(indent + 4) + "\n";
        return s;
    }
};

class ElseStmt : public Statement {
public:
    std::unique_ptr<ASTNode> elseBranch;
    explicit ElseStmt(std::unique_ptr<ASTNode> elseBr)
        : Statement(ASTNodeType::ELSE_STMT), elseBranch(std::move(elseBr)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ElseStmt\n" + (elseBranch ? elseBranch->toString(indent + 2) : "");
    }
};

class WhileStmt : public Statement {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    WhileStmt()
        : Statement(ASTNodeType::WHILE_STMT) {}
    WhileStmt(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : Statement(ASTNodeType::WHILE_STMT), condition(std::move(cond)), body(std::move(b)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "WhileStmt\n";
        if (condition) s += std::string(indent + 2, ' ') + "Condition: " + condition->toString(0) + "\n";
        if (body) s += std::string(indent + 2, ' ') + "Body: " + body->toString(indent + 4) + "\n";
        return s;
    }
};

class DoWhileStmt : public Statement {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    DoWhileStmt(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : Statement(ASTNodeType::DO_WHILE_STMT), condition(std::move(cond)), body(std::move(b)) {}
    DoWhileStmt() : Statement(ASTNodeType::DO_WHILE_STMT) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "DoWhileStmt\n";
        if (body) s += std::string(indent + 2, ' ') + "Body: " + body->toString(indent + 4) + "\n";
        if (condition) s += std::string(indent + 2, ' ') + "Condition: " + condition->toString(0) + "\n";
        return s;
    }
};

class ForStmt : public Statement {
public:
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;
    ForStmt()
        : Statement(ASTNodeType::FOR_STMT) {}
    ForStmt(std::unique_ptr<ASTNode> i, std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> b)
        : Statement(ASTNodeType::FOR_STMT), init(std::move(i)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(b)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "ForStmt\n";
        if (init) s += std::string(indent + 2, ' ') + "Init: " + init->toString(0) + "\n";
        if (condition) s += std::string(indent + 2, ' ') + "Condition: " + condition->toString(0) + "\n";
        if (increment) s += std::string(indent + 2, ' ') + "Increment: " + increment->toString(0) + "\n";
        if (body) s += std::string(indent + 2, ' ') + "Body: " + body->toString(indent + 4) + "\n";
        return s;
    }
};

class ReturnStmt : public Statement {
public:
    std::unique_ptr<ASTNode> expression;
    ReturnStmt()
        : Statement(ASTNodeType::RETURN_STMT) {}
    explicit ReturnStmt(std::unique_ptr<ASTNode> expr)
        : Statement(ASTNodeType::RETURN_STMT), expression(std::move(expr)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ReturnStmt(" + (expression ? expression->toString(0) : "null") + ")\n";
    }
};

class BreakStmt : public Statement {
public:
    BreakStmt() : Statement(ASTNodeType::BREAK_STMT) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "BreakStmt\n";
    }
};

class ContinueStmt : public Statement {
public:
    ContinueStmt() : Statement(ASTNodeType::CONTINUE_STMT) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ContinueStmt\n";
    }
};

class GotoStmt : public Statement {
public:
    std::string label;
    explicit GotoStmt(std::string lbl)
        : Statement(ASTNodeType::GOTO_STMT), label(std::move(lbl)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "GotoStmt: " + label + "\n";
    }
};

class CatchStmt : public Statement {
public:
    std::unique_ptr<ASTNode> exceptionType;
    std::string exceptionVar;
    std::unique_ptr<BlockStmt> body;
    CatchStmt(std::unique_ptr<ASTNode> type, std::string var, std::unique_ptr<BlockStmt> b)
        : Statement(ASTNodeType::CATCH_STMT), exceptionType(std::move(type)), exceptionVar(std::move(var)), body(std::move(b)) {}
    CatchStmt() : Statement(ASTNodeType::CATCH_STMT) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "CatchStmt: " + exceptionVar + "\n";
        if (exceptionType) s += std::string(indent + 2, ' ') + "Type: " + exceptionType->toString(0) + "\n";
        if (body) s += body->toString(indent + 2) + "\n";
        return s;
    }
};

class TryStmt : public Statement {
public:
    std::unique_ptr<BlockStmt> tryBlock;
    std::vector<std::unique_ptr<CatchStmt>> catchClauses;
    TryStmt(std::unique_ptr<BlockStmt> block, std::vector<std::unique_ptr<CatchStmt>> catches)
        : Statement(ASTNodeType::TRY_STMT), tryBlock(std::move(block)), catchClauses(std::move(catches)) {}
    TryStmt() : Statement(ASTNodeType::TRY_STMT) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "TryStmt\n";
        if (tryBlock) s += tryBlock->toString(indent + 2) + "\n";
        for (const auto& c : catchClauses) if (c) s += c->toString(indent + 2) + "\n";
        return s;
    }
};

class ThrowStmt : public Statement {
public:
    std::unique_ptr<ASTNode> expression;
    explicit ThrowStmt(std::unique_ptr<ASTNode> expr)
        : Statement(ASTNodeType::THROW_STMT), expression(std::move(expr)) {}
    ThrowStmt() : Statement(ASTNodeType::THROW_STMT) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ThrowStmt(" + (expression ? expression->toString(0) : "null") + ")\n";
    }
};

class SwitchStmt : public Statement {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> cases;
    SwitchStmt(std::unique_ptr<ASTNode> cond, std::vector<std::unique_ptr<ASTNode>> caseList)
        : Statement(ASTNodeType::SWITCH_STMT), condition(std::move(cond)), cases(std::move(caseList)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "SwitchStmt\n";
        if (condition) s += std::string(indent + 2, ' ') + "Condition: " + condition->toString(0) + "\n";
        for (const auto& c : cases) if (c) s += c->toString(indent + 2) + "\n";
        return s;
    }
};

class CaseStmt : public Statement {
public:
    std::unique_ptr<ASTNode> value;
    std::vector<std::unique_ptr<ASTNode>> statements;
    CaseStmt(std::unique_ptr<ASTNode> val, std::vector<std::unique_ptr<ASTNode>> stmts)
        : Statement(ASTNodeType::CASE_STMT), value(std::move(val)), statements(std::move(stmts)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "CaseStmt\n";
        if (value) s += std::string(indent + 2, ' ') + "Value: " + value->toString(0) + "\n";
        for (const auto& stmt : statements) if (stmt) s += stmt->toString(indent + 2) + "\n";
        return s;
    }
};

class DefaultStmt : public Statement {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    explicit DefaultStmt(std::vector<std::unique_ptr<ASTNode>> stmts)
        : Statement(ASTNodeType::DEFAULT_STMT), statements(std::move(stmts)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "DefaultStmt\n";
        for (const auto& stmt : statements) if (stmt) s += stmt->toString(indent + 2) + "\n";
        return s;
    }
};



class Expression : public ASTNode {
public:
    explicit Expression(ASTNodeType type) : ASTNode(type) {}
};

class InitializerListExpr : public Expression {
public:
    std::vector<std::unique_ptr<ASTNode>> elements;
    InitializerListExpr(std::vector<std::unique_ptr<ASTNode>> elems)
        : Expression(ASTNodeType::INITIALIZER_LIST_EXPR), elements(std::move(elems)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "InitializerListExpr\n";
        for (const auto& e : elements) if (e) s += e->toString(indent + 2) + ", ";
        return s;
    }
};

class BinaryExpr : public Expression {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryExpr(std::string oper, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : Expression(ASTNodeType::BINARY_EXPR), op(std::move(oper)),
          left(std::move(lhs)), right(std::move(rhs)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "BinaryExpr: " + op + "\n";
        if (left) s += left->toString(indent + 2) + "\n";
        if (right) s += right->toString(indent + 2);
        return s;
    }
};

class UnaryExpr : public Expression {
public:
    std::string op;
    std::unique_ptr<ASTNode> operand;
    bool isPrefix;
    UnaryExpr(std::string oper, std::unique_ptr<ASTNode> opd, bool prefix)
        : Expression(ASTNodeType::UNARY_EXPR), op(std::move(oper)), operand(std::move(opd)), isPrefix(prefix) {}
    UnaryExpr(std::string oper, std::unique_ptr<ASTNode> opd)
        : Expression(ASTNodeType::UNARY_EXPR), op(std::move(oper)), operand(std::move(opd)), isPrefix(true) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "UnaryExpr: " + op + (operand ? (" " + operand->toString(0)) : "");
    }
};

class TernaryExpr : public Expression {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> trueExpr;
    std::unique_ptr<ASTNode> falseExpr;
    TernaryExpr(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> t, std::unique_ptr<ASTNode> f)
        : Expression(ASTNodeType::TERNARY_EXPR),
          condition(std::move(cond)), trueExpr(std::move(t)), falseExpr(std::move(f)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "TernaryExpr\n";
        if (condition) s += std::string(indent + 2, ' ') + "Cond: " + condition->toString(0) + "\n";
        if (trueExpr) s += std::string(indent + 2, ' ') + "True: " + trueExpr->toString(0) + "\n";
        if (falseExpr) s += std::string(indent + 2, ' ') + "False: " + falseExpr->toString(0) + "\n";
        return s;
    }
};



class Literal : public Expression {
public:
    std::string value;
    std::string literalType;
    explicit Literal(std::string val)
        : Expression(ASTNodeType::LITERAL), value(std::move(val)), literalType("") {}
    Literal(std::string val, std::string type)
        : Expression(ASTNodeType::LITERAL), value(std::move(val)), literalType(std::move(type)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "Literal: " + literalType + " " + value;
    }
};

class Identifier : public Expression {
public:
    std::string name;
    explicit Identifier(std::string idName)
        : Expression(ASTNodeType::IDENTIFIER), name(std::move(idName)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "Identifier: " + name;
    }
};

class FunctionCall : public Expression {
public:
    std::unique_ptr<ASTNode> callee;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    std::vector<std::unique_ptr<ASTNode>> templateArgs;
    FunctionCall(std::unique_ptr<ASTNode> calleeNode,
                 std::vector<std::unique_ptr<ASTNode>> args = {},
                 std::vector<std::unique_ptr<ASTNode>> targs = {})
        : Expression(ASTNodeType::FUNCTION_CALL),
          callee(std::move(calleeNode)),
          arguments(std::move(args)),
          templateArgs(std::move(targs)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "FunctionCall\n";
        if (callee) s += std::string(indent + 2, ' ') + "Callee: " + callee->toString(0) + "\n";
        if (!arguments.empty()) {
            s += std::string(indent + 2, ' ') + "Args: ";
            for (const auto& arg : arguments) if (arg) s += arg->toString(0) + ", ";
            s += "\n";
        }
        if (!templateArgs.empty()) {
            s += std::string(indent + 2, ' ') + "TemplateArgs: ";
            for (const auto& t : templateArgs) if (t) s += t->toString(0) + ", ";
            s += "\n";
        }
        return s;
    }
};

class MemberAccess : public Expression {
public:
    std::unique_ptr<ASTNode> object;
    std::string memberName;
    bool isArrow;
    MemberAccess(std::unique_ptr<ASTNode> obj, std::string member, bool arrow)
        : Expression(ASTNodeType::MEMBER_ACCESS), object(std::move(obj)), memberName(std::move(member)), isArrow(arrow) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "MemberAccess: " + (object ? object->toString(0) : "null") + (isArrow ? "->" : ".") + memberName;
    }
};

class ArrayAccess : public Expression {
public:
    std::unique_ptr<ASTNode> arrayExpr;
    std::unique_ptr<ASTNode> indexExpr;
    ArrayAccess(std::unique_ptr<ASTNode> arr, std::unique_ptr<ASTNode> idx)
        : Expression(ASTNodeType::ARRAY_ACCESS), arrayExpr(std::move(arr)), indexExpr(std::move(idx)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ArrayAccess: " + (arrayExpr ? arrayExpr->toString(0) : "null") + "[" + (indexExpr ? indexExpr->toString(0) : "null") + "]";
    }
};

class VectorAccess : public Expression {
public:
    std::unique_ptr<ASTNode> vectorExpr;
    std::string method;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    VectorAccess(std::unique_ptr<ASTNode> vec, std::string m, std::vector<std::unique_ptr<ASTNode>> args)
        : Expression(ASTNodeType::VECTOR_ACCESS), vectorExpr(std::move(vec)), method(std::move(m)), arguments(std::move(args)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "VectorAccess: " + (vectorExpr ? vectorExpr->toString(0) : "null") + "." + method + "(";
        for (const auto& arg : arguments) if (arg) s += arg->toString(0) + ", ";
        s += ")";
        return s;
    }
};

class StreamExpr : public Expression {
public:
    std::vector<std::unique_ptr<ASTNode>> chain;
    StreamExpr(std::vector<std::unique_ptr<ASTNode>> c)
        : Expression(ASTNodeType::STREAM_EXPR), chain(std::move(c)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "StreamExpr: ";
        for (const auto& n : chain) if (n) s += n->toString(0) + " << ";
        return s;
    }
};

class LambdaExpr : public Expression {
public:
    std::vector<std::string> captureList;
    std::vector<std::unique_ptr<ASTNode>> parameters;
    std::unique_ptr<ASTNode> returnType;
    std::unique_ptr<ASTNode> body;
    LambdaExpr(std::vector<std::unique_ptr<ASTNode>> params, std::unique_ptr<ASTNode> b)
        : Expression(ASTNodeType::LAMBDA_EXPR), parameters(std::move(params)), body(std::move(b)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "LambdaExpr\n";
        if (!parameters.empty()) {
            s += std::string(indent + 2, ' ') + "Params: ";
            for (const auto& p : parameters) if (p) s += p->toString(0) + ", ";
            s += "\n";
        }
        if (body) s += std::string(indent + 2, ' ') + "Body: " + body->toString(indent + 4) + "\n";
        return s;
    }
};

class StaticCastExpr : public Expression {
public:
    std::unique_ptr<ASTNode> targetType;
    std::unique_ptr<ASTNode> expr;
    StaticCastExpr(std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> e)
        : Expression(ASTNodeType::STATIC_CAST_EXPR), targetType(std::move(type)), expr(std::move(e)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "StaticCastExpr<" + (targetType ? targetType->toString(0) : "null") + ", " + (expr ? expr->toString(0) : "null") + ">";
    }
};

class DynamicCastExpr : public Expression {
public:
    std::unique_ptr<ASTNode> targetType;
    std::unique_ptr<ASTNode> expr;
    DynamicCastExpr(std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> e)
        : Expression(ASTNodeType::DYNAMIC_CAST_EXPR), targetType(std::move(type)), expr(std::move(e)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "DynamicCastExpr<" + (targetType ? targetType->toString(0) : "null") + ", " + (expr ? expr->toString(0) : "null") + ">";
    }
};

class ConstCastExpr : public Expression {
public:
    std::unique_ptr<ASTNode> targetType;
    std::unique_ptr<ASTNode> expr;
    ConstCastExpr(std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> e)
        : Expression(ASTNodeType::CONST_CAST_EXPR), targetType(std::move(type)), expr(std::move(e)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ConstCastExpr<" + (targetType ? targetType->toString(0) : "null") + ", " + (expr ? expr->toString(0) : "null") + ">";
    }
};

class ReinterpretCastExpr : public Expression {
public:
    std::unique_ptr<ASTNode> targetType;
    std::unique_ptr<ASTNode> expr;
    ReinterpretCastExpr(std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> e)
        : Expression(ASTNodeType::REINTERPRET_CAST_EXPR), targetType(std::move(type)), expr(std::move(e)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ReinterpretCastExpr<" + (targetType ? targetType->toString(0) : "null") + ", " + (expr ? expr->toString(0) : "null") + ">";
    }
};

class TypeidExpr : public Expression {
public:
    std::unique_ptr<ASTNode> expr;
    TypeidExpr(std::unique_ptr<ASTNode> e)
        : Expression(ASTNodeType::TYPEID_EXPR), expr(std::move(e)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "TypeidExpr(" + (expr ? expr->toString(0) : "null") + ")";
    }
};

class TemplateType : public ASTNode {
public:
    std::string baseTypeName;
    std::vector<std::unique_ptr<ASTNode>> typeArgs;
    TemplateType(std::string base, std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::TEMPLATE_TYPE), baseTypeName(std::move(base)), typeArgs(std::move(args)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "TemplateType: " + baseTypeName + "<";
        for (const auto& t : typeArgs) if (t) s += t->toString(0) + ", ";
        s += ">";
        return s;
    }
};

class QualifiedType : public ASTNode {
public:
    std::string name;
    bool isConst = false;
    bool isPointer = false;
    bool isReference = false;
    QualifiedType(std::string n) : ASTNode(ASTNodeType::QUALIFIED_TYPE), name(std::move(n)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "QualifiedType: " + name;
        if (isConst) s += " const";
        if (isPointer) s += " *";
        if (isReference) s += " &";
        return s;
    }
};

class QualifiedName : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::string right;
    QualifiedName(std::unique_ptr<ASTNode> l, std::string r)
        : ASTNode(ASTNodeType::QUALIFIED_NAME), left(std::move(l)), right(std::move(r)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "QualifiedName: " + (left ? left->toString(0) : "null") + "::" + right;
    }
};

class PointerType : public ASTNode {
public:
    std::unique_ptr<ASTNode> baseType;
    PointerType(std::unique_ptr<ASTNode> base)
        : ASTNode(ASTNodeType::POINTER_TYPE), baseType(std::move(base)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PointerType: " + (baseType ? baseType->toString(0) : "null");
    }
};

class ReferenceType : public ASTNode {
public:
    std::unique_ptr<ASTNode> baseType;
    ReferenceType(std::unique_ptr<ASTNode> base)
        : ASTNode(ASTNodeType::REFERENCE_TYPE), baseType(std::move(base)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ReferenceType: " + (baseType ? baseType->toString(0) : "null");
    }
};

class AssignmentExpr : public Expression {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op;
    AssignmentExpr(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs, std::string oper)
        : Expression(ASTNodeType::BINARY_EXPR), left(std::move(lhs)), right(std::move(rhs)), op(std::move(oper)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "AssignmentExpr: " + op + "\n";
        if (left) s += left->toString(indent + 2) + "\n";
        if (right) s += right->toString(indent + 2);
        return s;
    }
};


// --- Preprocessor AST nodes ---
class PreprocessorInclude : public ASTNode {
public:
    std::string file;
    explicit PreprocessorInclude(std::string f)
        : ASTNode(ASTNodeType::PREPROCESSOR_INCLUDE), file(std::move(f)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorInclude: " + file;
    }
};
class PreprocessorDefine : public ASTNode {
public:
    std::string macro;
    std::string value;
    PreprocessorDefine(std::string m, std::string v)
        : ASTNode(ASTNodeType::PREPROCESSOR_DEFINE), macro(std::move(m)), value(std::move(v)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorDefine: " + macro + " = " + value;
    }
};
class PreprocessorUndef : public ASTNode {
public:
    std::string macro;
    explicit PreprocessorUndef(std::string m)
        : ASTNode(ASTNodeType::PREPROCESSOR_UNDEF), macro(std::move(m)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorUndef: " + macro;
    }
};
class PreprocessorIfdef : public ASTNode {
public:
    std::string macro;
    explicit PreprocessorIfdef(std::string m)
        : ASTNode(ASTNodeType::PREPROCESSOR_IFDEF), macro(std::move(m)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorIfdef: " + macro;
    }
};
class PreprocessorIfndef : public ASTNode {
public:
    std::string macro;
    explicit PreprocessorIfndef(std::string m)
        : ASTNode(ASTNodeType::PREPROCESSOR_IFNDEF), macro(std::move(m)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorIfndef: " + macro;
    }
};
class PreprocessorIf : public ASTNode {
public:
    std::string condition;
    explicit PreprocessorIf(std::string cond)
        : ASTNode(ASTNodeType::PREPROCESSOR_IF), condition(std::move(cond)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorIf: " + condition;
    }
};
class PreprocessorElse : public ASTNode {
public:
    PreprocessorElse() : ASTNode(ASTNodeType::PREPROCESSOR_ELSE) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorElse";
    }
};
class PreprocessorElif : public ASTNode {
public:
    std::string condition;
    explicit PreprocessorElif(std::string cond)
        : ASTNode(ASTNodeType::PREPROCESSOR_ELIF), condition(std::move(cond)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorElif: " + condition;
    }
};
class PreprocessorEndif : public ASTNode {
public:
    PreprocessorEndif() : ASTNode(ASTNodeType::PREPROCESSOR_ENDIF) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorEndif";
    }
};
class PreprocessorPragma : public ASTNode {
public:
    std::string pragma;
    explicit PreprocessorPragma(std::string p)
        : ASTNode(ASTNodeType::PREPROCESSOR_PRAGMA), pragma(std::move(p)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorPragma: " + pragma;
    }
};
class PreprocessorUnknown : public ASTNode {
public:
    std::string text;
    explicit PreprocessorUnknown(std::string t)
        : ASTNode(ASTNodeType::PREPROCESSOR_UNKNOWN), text(std::move(t)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "PreprocessorUnknown: " + text;
    }
};

// --- Template/Generic AST nodes ---
class TemplateParam : public ASTNode {
public:
    std::string name;
    bool isTypeParam;
    std::string defaultValue;
    TemplateParam(std::string n, bool isType, std::string def = "")
        : ASTNode(ASTNodeType::TEMPLATE_PARAM), name(std::move(n)), isTypeParam(isType), defaultValue(std::move(def)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "TemplateParam: " + name + (isTypeParam ? " (type)" : " (non-type)") + (defaultValue.empty() ? "" : (" = " + defaultValue));
    }
};
class TemplateClassDecl : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<TemplateParam>> templateParams;
    std::vector<std::unique_ptr<ASTNode>> members;
    TemplateClassDecl(std::string n, std::vector<std::unique_ptr<TemplateParam>> params, std::vector<std::unique_ptr<ASTNode>> mems)
        : ASTNode(ASTNodeType::TEMPLATE_CLASS_DECL), name(std::move(n)), templateParams(std::move(params)), members(std::move(mems)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "TemplateClassDecl: " + name + "\n";
        for (const auto& p : templateParams) if (p) s += p->toString(indent + 2) + "\n";
        for (const auto& m : members) if (m) s += m->toString(indent + 2) + "\n";
        return s;
    }
};
class TemplateFunctionDecl : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<TemplateParam>> templateParams;
    std::unique_ptr<ASTNode> returnType;
    std::vector<std::unique_ptr<ASTNode>> parameters;
    std::unique_ptr<ASTNode> body;
    TemplateFunctionDecl(std::string n, std::vector<std::unique_ptr<TemplateParam>> params, std::unique_ptr<ASTNode> ret, std::vector<std::unique_ptr<ASTNode>> paramList, std::unique_ptr<ASTNode> b)
        : ASTNode(ASTNodeType::TEMPLATE_FUNCTION_DECL), name(std::move(n)), templateParams(std::move(params)), returnType(std::move(ret)), parameters(std::move(paramList)), body(std::move(b)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "TemplateFunctionDecl: " + name + "\n";
        for (const auto& p : templateParams) if (p) s += p->toString(indent + 2) + "\n";
        if (returnType) s += std::string(indent + 2, ' ') + "ReturnType: " + returnType->toString(0) + "\n";
        for (const auto& param : parameters) if (param) s += param->toString(indent + 2) + "\n";
        if (body) s += body->toString(indent + 2) + "\n";
        return s;
    }
};

// --- Threading/Concurrency AST nodes ---
class ThreadDecl : public ASTNode {
public:
    std::string threadVarName;
    std::unique_ptr<ASTNode> callable;
    ThreadDecl(std::string name, std::unique_ptr<ASTNode> call)
        : ASTNode(ASTNodeType::THREAD_DECL), threadVarName(std::move(name)), callable(std::move(call)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ThreadDecl: " + threadVarName + (callable ? (" (callable: " + callable->toString(0) + ")") : "");
    }
};
class MutexDecl : public ASTNode {
public:
    std::string name;
    explicit MutexDecl(std::string n)
        : ASTNode(ASTNodeType::MUTEX_DECL), name(std::move(n)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "MutexDecl: " + name;
    }
};
class LockGuardDecl : public ASTNode {
public:
    std::string name;
    std::string mutexName;
    LockGuardDecl(std::string n, std::string m)
        : ASTNode(ASTNodeType::LOCK_GUARD_DECL), name(std::move(n)), mutexName(std::move(m)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "LockGuardDecl: " + name + " (mutex: " + mutexName + ")";
    }
};

// --- Math/Algorithm/IO/Memory AST nodes ---
class MathFunctionCall : public ASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    MathFunctionCall(std::string fname, std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::BINARY_EXPR), functionName(std::move(fname)), arguments(std::move(args)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "MathFunctionCall: " + functionName + "(";
        for (const auto& arg : arguments) if (arg) s += arg->toString(0) + ", ";
        s += ")";
        return s;
    }
};
class SortCall : public ASTNode {
public:
    std::unique_ptr<ASTNode> container;
    SortCall(std::unique_ptr<ASTNode> c)
        : ASTNode(ASTNodeType::SORT_CALL), container(std::move(c)) {}
    SortCall(std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::SORT_CALL) {
        if (args.size() == 1) {
            container = std::move(args[0]);
        } else {
            container = nullptr;
        }
    }
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "SortCall: " + (container ? container->toString(0) : "null");
    }
};
class FindCall : public ASTNode {
public:
    std::unique_ptr<ASTNode> container;
    std::unique_ptr<ASTNode> value;
    FindCall(std::unique_ptr<ASTNode> c, std::unique_ptr<ASTNode> v)
        : ASTNode(ASTNodeType::FIND_CALL), container(std::move(c)), value(std::move(v)) {}
    FindCall(std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::FIND_CALL) {
        if (args.size() == 2) {
            container = std::move(args[0]);
            value = std::move(args[1]);
        } else {
            container = nullptr;
            value = nullptr;
        }
    }
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "FindCall: " + (container ? container->toString(0) : "null") + ", value: " + (value ? value->toString(0) : "null");
    }
};
class AccumulateCall : public ASTNode {
public:
    std::unique_ptr<ASTNode> beginExpr;
    std::unique_ptr<ASTNode> endExpr;
    std::unique_ptr<ASTNode> initialValue;
    AccumulateCall(std::unique_ptr<ASTNode> b, std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> init)
        : ASTNode(ASTNodeType::ACCUMULATE_CALL), beginExpr(std::move(b)), endExpr(std::move(e)), initialValue(std::move(init)) {}
    AccumulateCall(std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::ACCUMULATE_CALL) {
        if (args.size() == 3) {
            beginExpr = std::move(args[0]);
            endExpr = std::move(args[1]);
            initialValue = std::move(args[2]);
        } else {
            beginExpr = nullptr;
            endExpr = nullptr;
            initialValue = nullptr;
        }
    }
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "AccumulateCall: begin=" + (beginExpr ? beginExpr->toString(0) : "null") + ", end=" + (endExpr ? endExpr->toString(0) : "null") + ", init=" + (initialValue ? initialValue->toString(0) : "null");
    }
};
class CoutExpr : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> outputValues;
    CoutExpr(std::vector<std::unique_ptr<ASTNode>> vals)
        : ASTNode(ASTNodeType::COUT_EXPR), outputValues(std::move(vals)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "CoutExpr: ";
        for (const auto& v : outputValues) if (v) s += v->toString(0) + ", ";
        return s;
    }
};
class CerrExpr : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> errorOutputs;
    CerrExpr(std::vector<std::unique_ptr<ASTNode>> vals)
        : ASTNode(ASTNodeType::CERR_EXPR), errorOutputs(std::move(vals)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "CerrExpr: ";
        for (const auto& v : errorOutputs) if (v) s += v->toString(0) + ", ";
        return s;
    }
};
class CinExpr : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> inputTargets;
    CinExpr(std::vector<std::unique_ptr<ASTNode>> targets)
        : ASTNode(ASTNodeType::CIN_EXPR), inputTargets(std::move(targets)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "CinExpr: ";
        for (const auto& v : inputTargets) if (v) s += v->toString(0) + ", ";
        return s;
    }
};
class GetlineCall : public ASTNode {
public:
    std::unique_ptr<ASTNode> targetVar;
    GetlineCall(std::unique_ptr<ASTNode> t)
        : ASTNode(ASTNodeType::GETLINE_CALL), targetVar(std::move(t)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "GetlineCall: " + (targetVar ? targetVar->toString(0) : "null");
    }
};
class PrintfCall : public ASTNode {
public:
    std::string format;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    PrintfCall(std::string fmt, std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::PRINTF_CALL), format(std::move(fmt)), arguments(std::move(args)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "PrintfCall: " + format + ", args: ";
        for (const auto& a : arguments) if (a) s += a->toString(0) + ", ";
        return s;
    }
};
class ScanfCall : public ASTNode {
public:
    std::string format;
    std::vector<std::unique_ptr<ASTNode>> inputTargets;
    ScanfCall(std::string fmt, std::vector<std::unique_ptr<ASTNode>> targets)
        : ASTNode(ASTNodeType::SCANF_CALL), format(std::move(fmt)), inputTargets(std::move(targets)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "ScanfCall: " + format + ", targets: ";
        for (const auto& t : inputTargets) if (t) s += t->toString(0) + ", ";
        return s;
    }
};
class MallocCall : public ASTNode {
public:
    std::unique_ptr<ASTNode> elementType;
    std::unique_ptr<ASTNode> sizeExpr;
    MallocCall(std::unique_ptr<ASTNode> elem, std::unique_ptr<ASTNode> size)
        : ASTNode(ASTNodeType::MALLOC_CALL), elementType(std::move(elem)), sizeExpr(std::move(size)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "MallocCall: type=" + (elementType ? elementType->toString(0) : "null") + ", size=" + (sizeExpr ? sizeExpr->toString(0) : "null");
    }
};
class FreeCall : public ASTNode {
public:
    std::unique_ptr<ASTNode> ptrExpr;
    explicit FreeCall(std::unique_ptr<ASTNode> ptr)
        : ASTNode(ASTNodeType::FREE_CALL), ptrExpr(std::move(ptr)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "FreeCall: " + (ptrExpr ? ptrExpr->toString(0) : "null");
    }
};
class AbsCall : public ASTNode {
public:
    std::unique_ptr<ASTNode> valueExpr;
    explicit AbsCall(std::unique_ptr<ASTNode> v)
        : ASTNode(ASTNodeType::ABS_CALL), valueExpr(std::move(v)) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "AbsCall: " + (valueExpr ? valueExpr->toString(0) : "null");
    }
};
class NewExpr : public ASTNode {
public:
    std::unique_ptr<ASTNode> type;
    std::vector<std::unique_ptr<ASTNode>> args;
    NewExpr(std::unique_ptr<ASTNode> t, std::vector<std::unique_ptr<ASTNode>> a)
        : ASTNode(ASTNodeType::NEW_EXPR), type(std::move(t)), args(std::move(a)) {}
    std::string toString(int indent = 0) const override {
        std::string s = std::string(indent, ' ') + "NewExpr: type=" + (type ? type->toString(0) : "null") + ", args: ";
        for (const auto& a : args) if (a) s += a->toString(0) + ", ";
        return s;
    }
};
class DeleteExpr : public ASTNode {
public:
    std::unique_ptr<ASTNode> ptrExpr;
    bool isArrayDelete;
    DeleteExpr(std::unique_ptr<ASTNode> ptr, bool arr = false)
        : ASTNode(ASTNodeType::DELETE_EXPR), ptrExpr(std::move(ptr)), isArrayDelete(arr) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "DeleteExpr: " + (ptrExpr ? ptrExpr->toString(0) : "null") + (isArrayDelete ? " (array)" : "");
    }
};

// --- ArrayType AST node ---
class ArrayType : public ASTNode {
public:
    std::unique_ptr<ASTNode> elementType;
    std::unique_ptr<ASTNode> sizeExpr;
    ArrayType(std::unique_ptr<ASTNode> elem, std::unique_ptr<ASTNode> size)
        : ASTNode(ASTNodeType::ARRAY_TYPE), elementType(std::move(elem)), sizeExpr(std::move(size)) {}
    ArrayType(std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(ASTNodeType::ARRAY_TYPE) {
        if (args.size() == 2) {
            elementType = std::move(args[0]);
            sizeExpr = std::move(args[1]);
        } else {
            elementType = nullptr;
            sizeExpr = nullptr;
        }
    }
    ArrayType(std::unique_ptr<ASTNode> elem, const std::string& sizeStr)
        : ASTNode(ASTNodeType::ARRAY_TYPE), elementType(std::move(elem)), sizeExpr(std::make_unique<Literal>(sizeStr, "int")) {}
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ArrayType: " + (elementType ? elementType->toString(0) : "null") + "[" + (sizeExpr ? sizeExpr->toString(0) : "null") + "]";
    }
};

#endif
