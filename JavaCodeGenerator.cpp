#include "JavaCodeGenerator.hpp"
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <map>
#include <vector>
#include <algorithm> // For std::find


JavaCodeGenerator::JavaCodeGenerator() {
    logFile.open("OUTPUT/jcg_logs.txt", std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open OUTPUT/jcg_logs.txt for writing");
    }
    logFile << "[JCG] Log file opened successfully.\n";
}
JavaCodeGenerator::~JavaCodeGenerator() {
    if (logFile.is_open()) {
        logFile.close();
    }
}


// --- STL Container Method Mapping Table ---
static const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> stlMethodMap = {
    {"ArrayList", {
        {"push_back", "add"},
        {"pop_back", "remove"}, // Java: remove(size()-1)
        {"size", "size"},
        {"empty", "isEmpty"},
        {"clear", "clear"},
        {"at", "get"},
        {"front", "get"}, // Java: get(0)
        {"back", "get"},  // Java: get(size()-1)
        {"insert", "add"}, // Java: add(index, value)
        {"erase", "remove"}, // Java: remove(index)
        {"begin", "iterator"},
        {"end", "iterator"},
    }},
    {"LinkedList", {
        {"push_back", "addLast"},
        {"push_front", "addFirst"},
        {"pop_back", "removeLast"},
        {"pop_front", "removeFirst"},
        {"size", "size"},
        {"empty", "isEmpty"},
        {"clear", "clear"},
        {"front", "getFirst"},
        {"back", "getLast"},
        {"insert", "add"},
        {"erase", "remove"},
    }},
    {"HashSet", {
        {"insert", "add"},
        {"erase", "remove"},
        {"find", "contains"},
        {"count", "contains"},
        {"size", "size"},
        {"empty", "isEmpty"},
        {"clear", "clear"},
    }},
    {"HashMap", {
        {"insert", "put"},
        {"erase", "remove"},
        {"find", "containsKey"},
        {"count", "containsKey"},
        {"size", "size"},
        {"empty", "isEmpty"},
        {"clear", "clear"},
        {"at", "get"},
        {"[]", "get"},
    }},
    {"Stack", {
        {"push", "push"},
        {"pop", "pop"},
        {"top", "peek"},
        {"size", "size"},
        {"empty", "isEmpty"},
        {"clear", "clear"},
    }},
    {"Queue", {
        {"push", "add"},
        {"pop", "remove"},
        {"front", "peek"},
        {"size", "size"},
        {"empty", "isEmpty"},
        {"clear", "clear"},
    }},
    {"PriorityQueue", {
        {"push", "add"},
        {"pop", "remove"},
        {"top", "peek"},
        {"size", "size"},
        {"empty", "isEmpty"},
        {"clear", "clear"},
    }},
    {"BitSet", {
        {"set", "set"},
        {"reset", "clear"},
        {"flip", "flip"},
        {"size", "size"},
        {"count", "cardinality"},
        {"any", "length"},
        {"none", "isEmpty"},
    }}
};

// Helper: Map ASTNodeType to string for debug
static std::string astNodeTypeToString(ASTNodeType type) {
    switch(type) {
        case ASTNodeType::PROGRAM: return "PROGRAM";
        case ASTNodeType::USING_DIRECTIVE: return "USING_DIRECTIVE";
        case ASTNodeType::CLASS_DECL: return "CLASS_DECL";
        case ASTNodeType::STRUCT_DECL: return "STRUCT_DECL";
        case ASTNodeType::ENUM_DECL: return "ENUM_DECL";
        case ASTNodeType::FUNCTION_DECL: return "FUNCTION_DECL";
        case ASTNodeType::VAR_DECL: return "VAR_DECL";
        case ASTNodeType::BLOCK_STMT: return "BLOCK_STMT";
        case ASTNodeType::IF_STMT: return "IF_STMT";
        case ASTNodeType::FOR_STMT: return "FOR_STMT";
        case ASTNodeType::WHILE_STMT: return "WHILE_STMT";
        case ASTNodeType::DO_WHILE_STMT: return "DO_WHILE_STMT";
        case ASTNodeType::SWITCH_STMT: return "SWITCH_STMT";
        case ASTNodeType::CASE_STMT: return "CASE_STMT";
        case ASTNodeType::DEFAULT_STMT: return "DEFAULT_STMT";
        case ASTNodeType::BREAK_STMT: return "BREAK_STMT";
        case ASTNodeType::CONTINUE_STMT: return "CONTINUE_STMT";
        case ASTNodeType::RETURN_STMT: return "RETURN_STMT";
        case ASTNodeType::EXPRESSION_STMT: return "EXPRESSION_STMT";
        case ASTNodeType::BINARY_EXPR: return "BINARY_EXPR";
        case ASTNodeType::UNARY_EXPR: return "UNARY_EXPR";
        case ASTNodeType::TERNARY_EXPR: return "TERNARY_EXPR";
        case ASTNodeType::LITERAL: return "LITERAL";
        case ASTNodeType::FUNCTION_CALL: return "FUNCTION_CALL";
        case ASTNodeType::MEMBER_ACCESS: return "MEMBER_ACCESS";
        case ASTNodeType::ARRAY_ACCESS: return "ARRAY_ACCESS";
        case ASTNodeType::COUT_EXPR: return "COUT_EXPR";
        case ASTNodeType::CERR_EXPR: return "CERR_EXPR";
        case ASTNodeType::CIN_EXPR: return "CIN_EXPR";
        case ASTNodeType::GETLINE_CALL: return "GETLINE_CALL";
        case ASTNodeType::PRINTF_CALL: return "PRINTF_CALL";
        case ASTNodeType::SCANF_CALL: return "SCANF_CALL";
        case ASTNodeType::MALLOC_CALL: return "MALLOC_CALL";
        case ASTNodeType::FREE_CALL: return "FREE_CALL";
        case ASTNodeType::ABS_CALL: return "ABS_CALL";
        case ASTNodeType::TEMPLATE_CLASS_DECL: return "TEMPLATE_CLASS_DECL";
        case ASTNodeType::TEMPLATE_FUNCTION_DECL: return "TEMPLATE_FUNCTION_DECL";
        case ASTNodeType::INITIALIZER_LIST_EXPR: return "INITIALIZER_LIST_EXPR";
        case ASTNodeType::THREAD_DECL: return "THREAD_DECL";
        case ASTNodeType::MUTEX_DECL: return "MUTEX_DECL";
        case ASTNodeType::LOCK_GUARD_DECL: return "LOCK_GUARD_DECL";
        case ASTNodeType::LAMBDA_EXPR: return "LAMBDA_EXPR";
        case ASTNodeType::TRY_STMT: return "TRY_STMT";
        case ASTNodeType::CATCH_STMT: return "CATCH_STMT";
        case ASTNodeType::THROW_STMT: return "THROW_STMT";
        case ASTNodeType::NEW_EXPR: return "NEW_EXPR";
        case ASTNodeType::DELETE_EXPR: return "DELETE_EXPR";
        case ASTNodeType::PREPROCESSOR_DIRECTIVE: return "PREPROCESSOR_DIRECTIVE";
        case ASTNodeType::PREPROCESSOR_INCLUDE: return "PREPROCESSOR_INCLUDE";
        case ASTNodeType::PREPROCESSOR_DEFINE: return "PREPROCESSOR_DEFINE";
        case ASTNodeType::PREPROCESSOR_IFDEF: return "PREPROCESSOR_IFDEF";
        case ASTNodeType::PREPROCESSOR_IFNDEF: return "PREPROCESSOR_IFNDEF";
        case ASTNodeType::PREPROCESSOR_IF: return "PREPROCESSOR_IF";
        case ASTNodeType::PREPROCESSOR_ELIF: return "PREPROCESSOR_ELIF";
        case ASTNodeType::PREPROCESSOR_ELSE: return "PREPROCESSOR_ELSE";
        case ASTNodeType::PREPROCESSOR_ENDIF: return "PREPROCESSOR_ENDIF";
        case ASTNodeType::PREPROCESSOR_UNDEF: return "PREPROCESSOR_UNDEF";
        case ASTNodeType::PREPROCESSOR_PRAGMA: return "PREPROCESSOR_PRAGMA";
        case ASTNodeType::PREPROCESSOR_UNKNOWN: return "PREPROCESSOR_UNKNOWN";
        case ASTNodeType::NAMESPACE_DECL: return "NAMESPACE_DECL";
        case ASTNodeType::SQRT_CALL: return "SQRT_CALL";
        case ASTNodeType::POW_CALL: return "POW_CALL";
        case ASTNodeType::SORT_CALL: return "SORT_CALL";
        case ASTNodeType::FIND_CALL: return "FIND_CALL";
        case ASTNodeType::ACCUMULATE_CALL: return "ACCUMULATE_CALL";
        case ASTNodeType::VECTOR_ACCESS: return "VECTOR_ACCESS";
        case ASTNodeType::ASSIGNMENT_EXPR: return "ASSIGNMENT_EXPR";
        case ASTNodeType::IDENTIFIER: return "IDENTIFIER";
        // case ASTNodeType::CONDITION: return "CONDITION";
        default: return "UNKNOWN/INVALID";
    }
}

// --- Program Node ---
std::string JavaCodeGenerator::generateProgram(const Program* node, const std::string& className) {
    logFile << "[JCG] Starting Java code generation for class: " << className << std::endl;
    std::ostringstream oss;
    // Emit required imports at the top
    for (const auto& imp : requiredImports) {
        oss << imp << "\n";
    }
    oss << "\n";
    oss << "public class " << className << " {\n";
    int idx = 0;
    for (const auto& global : node->globals) {
        if (!global) {
            logFile << "[JCG][ERROR] Null global node at index " << idx << std::endl;
            continue;
        }
        logFile << "[JCG] Generating code for global node type: " << astNodeTypeToString(global->type) << std::endl;
        std::string code = generate(global.get(), className);
        // Indent each line of code inside the class
        std::istringstream codeStream(code);
        std::string line;
        while (std::getline(codeStream, line)) {
            if (!line.empty())
                oss << "    " << line << "\n";
            else
                oss << "\n";
        }
        idx++;
    }
    oss << "}\n";
    logFile << "[JCG] Finished generating Java program for class: " << className << std::endl;
    return oss.str();
}

// --- Main dispatcher ---
std::string JavaCodeGenerator::generate(const ASTNode* node, const std::string& className) {
    if (!node) {
        logFile << "[JCG][ERROR] generate called with null node" << std::endl;
        return "";
    }
    logFile << "[JCG] Generating node type: " << astNodeTypeToString(node->type) << std::endl;
    logFile.flush();
    std::cout << "[JCG] Generating node type: " << astNodeTypeToString(node->type)  << std::endl;
    switch (node->type) {
        case ASTNodeType::FUNCTION_DECL:
            return generateFunctionDecl(static_cast<const FunctionDecl*>(node), className);
        case ASTNodeType::VAR_DECL:
            return generateVarDecl(static_cast<const VarDecl*>(node), className);
        case ASTNodeType::BLOCK_STMT:
            return generateBlockStmt(static_cast<const BlockStmt*>(node), className);
        case ASTNodeType::IF_STMT:
            return generateIfStmt(static_cast<const IfStmt*>(node), className);
        case ASTNodeType::RETURN_STMT:
            return generateReturnStmt(static_cast<const ReturnStmt*>(node), className);
        case ASTNodeType::ASSIGNMENT_EXPR:
            // Treat assignment as a binary expression (op = "=")
            return generateAssignmentExpr(static_cast<const AssignmentExpr*>(node), className);
        case ASTNodeType::BINARY_EXPR:
            return generateBinaryExpr(static_cast<const BinaryExpr*>(node), className);
        case ASTNodeType::LITERAL:
            return generateLiteral(static_cast<const Literal*>(node));
        case ASTNodeType::IDENTIFIER:
            return generateIdentifier(static_cast<const Identifier*>(node));
        case ASTNodeType::CLASS_DECL:
            return generateClassDecl(static_cast<const ClassDecl*>(node), className);
        case ASTNodeType::STRUCT_DECL:
            return generateStructDecl(static_cast<const StructDecl*>(node), className);
        case ASTNodeType::ENUM_DECL:
            return generateEnumDecl(static_cast<const EnumDecl*>(node));
        case ASTNodeType::FOR_STMT:
            return generateForStmt(static_cast<const ForStmt*>(node), className);
        case ASTNodeType::WHILE_STMT:
            return generateWhileStmt(static_cast<const WhileStmt*>(node), className);
        case ASTNodeType::DO_WHILE_STMT:
            return generateDoWhileStmt(static_cast<const DoWhileStmt*>(node), className);
        case ASTNodeType::BREAK_STMT:
            return generateBreakStmt(static_cast<const BreakStmt*>(node), className);
        case ASTNodeType::CONTINUE_STMT:
            return generateContinueStmt(static_cast<const ContinueStmt*>(node), className);
        case ASTNodeType::EXPRESSION_STMT:
            return generateExpressionStmt(static_cast<const ExpressionStmt*>(node), className);
        case ASTNodeType::UNARY_EXPR:
            return generateUnaryExpr(static_cast<const UnaryExpr*>(node), className);
        case ASTNodeType::TERNARY_EXPR:
            return generateTernaryExpr(static_cast<const TernaryExpr*>(node), className);
        case ASTNodeType::FUNCTION_CALL:
            return generateFunctionCall(static_cast<const FunctionCall*>(node), className);
        case ASTNodeType::MEMBER_ACCESS:
            return generateMemberAccess(static_cast<const MemberAccess*>(node), className);
        case ASTNodeType::ARRAY_ACCESS:
            return generateArrayAccess(static_cast<const ArrayAccess*>(node), className);
        case ASTNodeType::SWITCH_STMT:
            return generateSwitchStmt(static_cast<const SwitchStmt*>(node), className);
        case ASTNodeType::CASE_STMT:
            return generateCaseStmt(static_cast<const CaseStmt*>(node), className);
        case ASTNodeType::DEFAULT_STMT:
            return generateDefaultStmt(static_cast<const DefaultStmt*>(node), className);
        case ASTNodeType::SORT_CALL:
            return generateSortCall(static_cast<const SortCall*>(node), className);
        case ASTNodeType::FIND_CALL:
            return generateFindCall(static_cast<const FindCall*>(node), className);
        case ASTNodeType::ACCUMULATE_CALL:
            return generateAccumulateCall(static_cast<const AccumulateCall*>(node), className);
        case ASTNodeType::VECTOR_ACCESS:
            return generateVectorAccess(static_cast<const VectorAccess*>(node), className);
        case ASTNodeType::COUT_EXPR:
            return generateCoutExpr(static_cast<const CoutExpr*>(node), className);
        case ASTNodeType::CERR_EXPR:
            return generateCerrExpr(static_cast<const CerrExpr*>(node), className);
        case ASTNodeType::CIN_EXPR:
            return generateCinExpr(static_cast<const CinExpr*>(node), className);
        case ASTNodeType::GETLINE_CALL:
            return generateGetlineCall(static_cast<const GetlineCall*>(node), className);
        case ASTNodeType::PRINTF_CALL:
            return generatePrintfCall(static_cast<const PrintfCall*>(node), className);
        case ASTNodeType::SCANF_CALL:
            return generateScanfCall(static_cast<const ScanfCall*>(node), className);
        case ASTNodeType::MALLOC_CALL:
            return generateMallocCall(static_cast<const MallocCall*>(node), className);
        case ASTNodeType::FREE_CALL:
            return generateFreeCall(static_cast<const FreeCall*>(node), className);
        case ASTNodeType::ABS_CALL:
            return generateAbsCall(static_cast<const AbsCall*>(node), className);
        case ASTNodeType::TEMPLATE_CLASS_DECL:
            return generateTemplateClassDecl(static_cast<const TemplateClassDecl*>(node), className);
        case ASTNodeType::TEMPLATE_FUNCTION_DECL:
            return generateTemplateFunctionDecl(static_cast<const TemplateFunctionDecl*>(node), className);
        case ASTNodeType::INITIALIZER_LIST_EXPR:
            return generateInitializerListExpr(static_cast<const InitializerListExpr*>(node), className);
        case ASTNodeType::THREAD_DECL:
            return generateThreadDecl(static_cast<const ThreadDecl*>(node), className);
        case ASTNodeType::MUTEX_DECL:
            return generateMutexDecl(static_cast<const MutexDecl*>(node), className);
        case ASTNodeType::LOCK_GUARD_DECL:
            return generateLockGuardDecl(static_cast<const LockGuardDecl*>(node), className);
        // case ASTNodeType::FIND_IF_CALL:
        //     return generateFindIfCall(static_cast<const FindCall*>(node), className);
        case ASTNodeType::LAMBDA_EXPR:
            return generateLambdaExpr(static_cast<const LambdaExpr*>(node), className);
        case ASTNodeType::TRY_STMT:
            return generateTryStmt(static_cast<const TryStmt*>(node), className);
        case ASTNodeType::THROW_STMT:
            return generateThrowStmt(static_cast<const ThrowStmt*>(node), className);
        case ASTNodeType::NEW_EXPR:
            return generateNewExpr(static_cast<const NewExpr*>(node), className);
        case ASTNodeType::DELETE_EXPR:
            return generateDeleteExpr(static_cast<const DeleteExpr*>(node), className);
        case ASTNodeType::PREPROCESSOR_DIRECTIVE:
        case ASTNodeType::PREPROCESSOR_INCLUDE:
        case ASTNodeType::PREPROCESSOR_DEFINE:
        case ASTNodeType::PREPROCESSOR_IFDEF:
        case ASTNodeType::PREPROCESSOR_IFNDEF:
        case ASTNodeType::PREPROCESSOR_IF:
        case ASTNodeType::PREPROCESSOR_ELIF:
        case ASTNodeType::PREPROCESSOR_ELSE:
        case ASTNodeType::PREPROCESSOR_ENDIF:
        case ASTNodeType::PREPROCESSOR_UNDEF:
        case ASTNodeType::PREPROCESSOR_PRAGMA:
        case ASTNodeType::PREPROCESSOR_UNKNOWN:
            return generatePreprocessorDirective(static_cast<const PreprocessorDirective*>(node));
        case ASTNodeType::NAMESPACE_DECL:
            return generateNamespaceDecl(static_cast<const NamespaceDecl*>(node), className);
        case ASTNodeType::USING_DIRECTIVE:
            return generateUsingDirective(static_cast<const UsingDirective*>(node), className);
        case ASTNodeType::CATCH_STMT:
            return generateCatchStmt(static_cast<const CatchStmt*>(node), className);
        case ASTNodeType::SQRT_CALL:
            return generateSqrtCall(static_cast<const MathFunctionCall*>(node), className);
        case ASTNodeType::POW_CALL:
            return generatePowCall(static_cast<const MathFunctionCall*>(node), className);
        case ASTNodeType::PROGRAM:
            // For the root, call generateProgram
            return generateProgram(static_cast<const Program*>(node), className);
        default:
            logFile << "[JCG][ERROR] Unsupported or invalid AST node type: (raw value), astNodeTypeToString: '" << astNodeTypeToString(node->type) << "'\n";
            return "";
    }
}


std::string JavaCodeGenerator::generateExpressionStmt(const ExpressionStmt* node, const std::string& className)  {
    std::string code = generate(node->expression.get(), className) + ";";
    if (//useCppExprFallback 
        !node->cppExpr.empty()) {
        return node->cppExpr; // fallback to original C++ code
    }

    // if (node->expression && node->expression->type == ASTNodeType::ASSIGNMENT_EXPR) {
    //     // Call dedicated assignment generator
    //     return generateAssignmentExpr(static_cast<const AssignmentExpr*>(node->expression.get()), className) + ";";
    // }
    return code;
}

std::string JavaCodeGenerator::generateAssignmentExpr(const AssignmentExpr* node, const std::string& className) {
    //impelement
    std::cout<<" assignment"  <<std::endl<< std::endl;

}

std::string JavaCodeGenerator::generateBinaryExpr(const BinaryExpr* node, const std::string& className) {
    if (!node) {
        logFile << "[JCG][ERROR] generateBinaryExpr called with null node" << std::endl;
        return "";
    }
    if (!node->left) {
        logFile << "[JCG][ERROR] generateBinaryExpr: left operand is null for op '" ;//<< node->op << "'" << std::endl;
        return "";
    }
    if (!node->right && node->op != "=") { // allow assignment with missing right for error reporting
        logFile << "[JCG][ERROR] generateBinaryExpr: right operand is null for op '" ;//<< node->op << "'" << std::endl;
        return "";
    }
    if (node->op.empty()) {
        logFile << "[JCG][ERROR] generateBinaryExpr: op is empty" << std::endl;
        return "";
    }
    // Special case: map[key] = value  ==>  map.put(key, value)
    if (node->op == "=" && node->left && node->left->type == ASTNodeType::ARRAY_ACCESS) {
        const ArrayAccess* arr = static_cast<const ArrayAccess*>(node->left.get());
        if (!arr->arrayExpr) {
            logFile << "[JCG][ERROR] generateBinaryExpr: arr->arrayExpr is null" << std::endl;
            return "";
        }
        // Try to get the type from the symbol table if arrayExpr is an Identifier
        const ASTNode* typeNode = nullptr;
        if (arr->arrayExpr->type == ASTNodeType::IDENTIFIER) {
            std::string varName = static_cast<const Identifier*>(arr->arrayExpr.get())->name;
            auto it = symbolTable.find(varName);
            if (it != symbolTable.end()) {
                typeNode = it->second;
            }
        }
        if (typeNode && typeNode->type == ASTNodeType::TEMPLATE_TYPE) {
            const TemplateType* tt = static_cast<const TemplateType*>(typeNode);
            std::string javaType = mapCppTypeNameToJava(tt->baseTypeName, false);
            if (javaType == "HashMap" || javaType == "Map") {
                logFile << "[JCG] Detected map assignment in generateBinaryExpr" << std::endl;
                return generate(arr->arrayExpr.get(), className) + ".put(" +
                       generate(arr->indexExpr.get(), className) + ", " +
                       generate(node->right.get(), className) + ")";
            }
        }
    }
    // Assignment operators
    static const std::vector<std::string> assignOps = {"=", "+=", "-=", "*=", "/=", "%="};
    std::string opStr = node->op;
    if (std::find(assignOps.begin(), assignOps.end(), opStr) != assignOps.end()) {
        std::string left = node->left ? generate(node->left.get(), className) : "";
        std::string right = node->right ? generate(node->right.get(), className) : "";
        logFile << "[JCG] Assignment op: '" << opStr << "', left: '" << left << "', right: '" << right << "'" << std::endl;
        if (right == "nullptr") right = "null";
        if (left == "nullptr") left = "null";
        return left + " " + opStr + " " + right;
    }
    // All other binary operators (arithmetic, logical, comparison)
    std::string left = generate(node->left.get(), className);
    std::string right = node->right ? generate(node->right.get(), className) : "";
    logFile << "[JCG] Binary op: '" << node->op << "', left: '" << left << "', right: '" << right << "'" << std::endl;
    if (right == "nullptr") right = "null";
    if (left == "nullptr") left = "null";
    return "(" + left + " " + node->op + " " + right + ")";
}

std::string JavaCodeGenerator::generateUnaryExpr(const UnaryExpr* node, const std::string& className)  {
    std::ostringstream oss;
    if (node->isPrefix) {
        oss << node->op << generate(node->operand.get(), className);
    } else {
        oss << generate(node->operand.get(), className) << node->op;
    }
    return oss.str();
}

std::string JavaCodeGenerator::generateTernaryExpr(const TernaryExpr* node, const std::string& className)  {
    std::ostringstream oss;
    oss << generate(node->condition.get(), className) << " ? "
        << generate(node->trueExpr.get(), className) << " : "
        << generate(node->falseExpr.get(), className);
    return oss.str();
}


// --- Literal ---
std::string JavaCodeGenerator::generateLiteral(const Literal* node)  {
    if (node->literalType == "float") {
        std::string val = node->value;
        if (val.empty() || (val.back() != 'f' && val.back() != 'F')) {
            val += "f";
        }
        return val;
    }
    if (node->literalType == "char" || node->literalType == "character") {
        // Ensure the value is wrapped in single quotes
        std::string val = node->value;
        if (val.size() == 1 || (val.size() == 2 && val[0] == '\\')) {
            // Single character or escape sequence, wrap in single quotes
            return std::string("'") + val + "'";
        }
        // If already wrapped, return as is
        if (val.size() >= 2 && val.front() == '\'' && val.back() == '\'') {
            return val;
        }
        // Fallback: wrap whatever is there
        return std::string("'") + val + "'";
    }
    if (node->literalType == "string" ) {
        // Ensure the value is wrapped in double quotes
        std::string val = node->value;
        if (val.empty() || (val.front() != '"' && val.back() != '"')) {
            return std::string("\"") + val + "\"";
        }
        return val; 
    }
    return node->value;
}

// --- Identifier ---
std::string JavaCodeGenerator::generateIdentifier(const Identifier* node)  {
    return node->name;
    
}

// --- Type Mapping: ASTNode* to Java type string ---

std::string JavaCodeGenerator::mapTypeNodeToJava(const ASTNode* typeNode, bool forGeneric) const {
    if (!typeNode) return "void";
    switch (typeNode->type) {
        case ASTNodeType::QUALIFIED_TYPE: {
            const QualifiedType* qt = static_cast<const QualifiedType*>(typeNode);
            return mapCppTypeNameToJava(qt->name, forGeneric);
        }
        case ASTNodeType::TEMPLATE_TYPE: {
            const TemplateType* tt = static_cast<const TemplateType*>(typeNode);
            std::ostringstream oss;
            oss << mapCppTypeNameToJava(tt->baseTypeName, forGeneric) << "<";
            for (size_t i = 0; i < tt->typeArgs.size(); ++i) {
                oss << mapTypeNodeToJava(tt->typeArgs[i].get(), true);
                if (i + 1 < tt->typeArgs.size()) oss << ", ";
            }
            oss << ">";
            return oss.str();
        }
        case ASTNodeType::POINTER_TYPE: {
            // Map C++ pointer to Java array if base type is primitive
            const auto* ptr = static_cast<const PointerType*>(typeNode);
            std::string base = mapTypeNodeToJava(ptr->baseType.get(), forGeneric);
            return base + "[]";
        }
        case ASTNodeType::REFERENCE_TYPE: {
            const auto* ref = static_cast<const ReferenceType*>(typeNode);
            return mapTypeNodeToJava(ref->baseType.get(), forGeneric);
        }
        case ASTNodeType::IDENTIFIER: {
            // Handle identifier as a type (primitive or user-defined)
            const Identifier* id = static_cast<const Identifier*>(typeNode);
            // Try to map to Java primitive/wrapper, else use as-is
            std::string mapped = mapCppTypeNameToJava(id->name, forGeneric);
            return mapped.empty() ? id->name : mapped;
        }
        default:
            return "Object";
    }
}

// --- Type Mapping: C++ type name to Java type name ---
std::string JavaCodeGenerator::mapCppTypeNameToJava(const std::string& cppType, bool forGeneric) const {
    static const std::unordered_map<std::string, std::string> primitiveMap = {
        {"int", "int"}, {"float", "float"}, {"double", "double"}, {"char", "char"}, {"bool", "boolean"}
    };
    static const std::unordered_map<std::string, std::string> wrapperMap = {
        {"int", "Integer"}, {"float", "Float"}, {"double", "Double"}, {"char", "Character"}, {"bool", "Boolean"}
    };
    static const std::unordered_map<std::string, std::string> typeMap = {
        {"std::string", "String"}, {"string", "String"}, {"void", "void"},
        {"vector", "ArrayList"}, {"std::vector", "ArrayList"},
        {"deque", "ArrayDeque"}, {"std::deque", "ArrayDeque"},
        {"list", "LinkedList"}, {"std::list", "LinkedList"},
        {"map", "HashMap"}, {"std::map", "HashMap"},
        {"unordered_map", "HashMap"}, {"std::unordered_map", "HashMap"},
        {"set", "HashSet"}, {"std::set", "HashSet"},
        {"unordered_set", "HashSet"}, {"std::unordered_set", "HashSet"},
        {"multimap", "HashMap"}, {"std::multimap", "HashMap"},
        {"multiset", "HashSet"}, {"std::multiset", "HashSet"},
        {"stack", "Stack"}, {"std::stack", "Stack"},
        {"queue", "Queue"}, {"std::queue", "Queue"},
        {"priority_queue", "PriorityQueue"}, {"std::priority_queue", "PriorityQueue"},
        {"bitset", "BitSet"}, {"std::bitset", "BitSet"},
        {"array", "ArrayList"}, {"std::array", "ArrayList"},
        {"pair", "AbstractMap.SimpleEntry"}, {"std::pair", "AbstractMap.SimpleEntry"},
        {"tuple", "Object[]"}, {"std::tuple", "Object[]"},
        {"optional", "Optional"}, {"std::optional", "Optional"},
        {"variant", "Object"}, {"std::variant", "Object"},
        {"any", "Object"}, {"std::any", "Object"},
    };

    if (forGeneric) {
        auto it = wrapperMap.find(cppType);
        if (it != wrapperMap.end()) return it->second;
    } else {
        auto it = primitiveMap.find(cppType);
        if (it != primitiveMap.end()) return it->second;
    }
    auto it = typeMap.find(cppType);
    if (it != typeMap.end()) return it->second;
    return cppType;
}

// --- Function Declaration ---
std::string JavaCodeGenerator::generateFunctionDecl(const FunctionDecl* node, const std::string& className, bool isClassMethod)  {
    logFile << "[JCG] Entering generateFunctionDecl for function: " << node->name << ", returnType: \n";// << mapTypeNodeToJava(node->returnType.get()) << std::endl;
    std::ostringstream oss;
    std::string returnType = mapTypeNodeToJava(node->returnType.get());
    if (!isClassMethod && !node->isConstructor && !node->isDestructor) {
        oss << "public static ";
    } 
    if (node->isVirtual) oss << "abstract ";
    if (!node->isConstructor && !node->isDestructor) {
        oss << returnType << " ";
    }
    if (node->isConstructor) {
        oss << className;
    } else if (node->isDestructor) {
        oss << "~" << className;
    } else {
        oss << node->name;
    }
    oss << "(";
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        const VarDecl* param = static_cast<const VarDecl*>(node->parameters[i].get());
        oss << mapTypeNodeToJava(param->type.get()) << " " << param->name;
        if (i + 1 < node->parameters.size()) oss << ", ";
    }
    oss << ")";
    if (node->isVirtual) {
        oss << ";";
    } else if (node->body) {
        oss << "{\n" << generate(node->body.get(), className) << "}";
    } else {
        oss << " {}";
    }
    logFile << "[JCG] Exiting generateFunctionDecl for function: " << node->name << std::endl;
    return oss.str();
}

// --- Variable Declaration ---
std::string JavaCodeGenerator::generateVarDecl(const VarDecl* node, const std::string& className) {
    logFile << "[JCG] Generating variable: " << node->name << ", type: \n" ;// << mapTypeNodeToJava(node->type.get()) << std::endl;
    std::ostringstream oss;
    std::string typeStr = mapTypeNodeToJava(node->type.get());
    oss << typeStr << " " << node->name;
    if (node->initializer) {
        oss << " = " << generate(node->initializer.get(), className);
    }
    oss << ";";
    return oss.str();
}

// --- Block Statement ---
std::string JavaCodeGenerator::generateBlockStmt(const BlockStmt* node, const std::string& className)  {
    std::ostringstream oss;
    for (const auto& stmt : node->statements) {
        oss << generate(stmt.get(), className) << "\n";
    }
    return oss.str();
}

// --- If Statement ---
std::string JavaCodeGenerator::generateIfStmt(const IfStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "if (" << generate(node->condition.get(), className) << ") ";
    if (node->thenBranch) {
        oss << "{\n" << generate(node->thenBranch.get(), className) << "}";
    }
    if (node->elseBranch) {
        oss << " else " << "{\n" << generate(node->elseBranch.get(), className) << "{\n";
    }
    return oss.str();
}

// --- Return Statement ---
std::string JavaCodeGenerator::generateReturnStmt(const ReturnStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "return";
    if (node->expression) {
        oss << " " << generate(node->expression.get(), className);
    }
    oss << ";";
    return oss.str();
}

// --- Namespace Declaration ---
std::string JavaCodeGenerator::generateNamespaceDecl(const NamespaceDecl* node, const std::string& className)  {
    std::ostringstream oss;
    for (const auto& member : node->declarations) {
        oss << generate(member.get(), className) << "\n";
    }
    return oss.str();
}

// --- Using Directive ---
std::string JavaCodeGenerator::generateUsingDirective(const UsingDirective* node, const std::string& /*className*/)  {
    std::ostringstream oss;
    oss << "// using directive: " << node->namespaceName << "\n";
    return oss.str();
}

// --- Class/Struct/Enum Translation ---
// ...existing code...

std::string JavaCodeGenerator::generateClassDecl(const ClassDecl* node, const std::string&) {
    std::ostringstream oss;
    bool isAbstract = false;
    // Check for virtual functions
    auto checkAbstract = [&](const std::vector<std::unique_ptr<ASTNode>>& members) {
        for (const auto& member : members) {
            if (member->type == ASTNodeType::FUNCTION_DECL) {
                const FunctionDecl* func = static_cast<const FunctionDecl*>(member.get());
                if (func->isVirtual) isAbstract = true;
            }
        }
    };
    checkAbstract(node->publicMembers);
    checkAbstract(node->privateMembers);
    checkAbstract(node->protectedMembers);

    oss << (isAbstract ? "public abstract class " : "public class ") << node->name;
    // Handle inheritance: only one base class in Java
    if (!node->bases.empty()) {
        oss << " extends " << node->bases[0].baseName;
        if (node->bases.size() > 1) {
            oss << " /* Multiple inheritance: ";
            for (size_t i = 1; i < node->bases.size(); ++i) {
                oss << node->bases[i].access << " " << node->bases[i].baseName;
                if (i + 1 < node->bases.size()) oss << ", ";
            }
            oss << " (ignored) */";
        }
    }
    oss << " {\n";
    auto emitMembers = [&](const std::vector<std::unique_ptr<ASTNode>>& members, const std::string& access) {
        for (const auto& member : members) {
            if (member->type == ASTNodeType::VAR_DECL) {
                oss << "    " << access << " " << generateVarDecl(static_cast<const VarDecl*>(member.get()), node->name) << "\n";
            } else if (member->type == ASTNodeType::FUNCTION_DECL) {
                const FunctionDecl* func = static_cast<const FunctionDecl*>(member.get());
                oss << "    " << access << " " << generateFunctionDecl(func, node->name, true) << "\n";
            }
        }
    };
    emitMembers(node->publicMembers, "public");
    emitMembers(node->protectedMembers, "protected");
    emitMembers(node->privateMembers, "private");
    oss << "}\n";
    return oss.str();
}

std::string JavaCodeGenerator::generateStructDecl(const StructDecl* node, const std::string&) {
    std::ostringstream oss;
    // In Java, struct is a class, but default all members to public
    oss << "public class " << node->name;
    if (!node->bases.empty()) {
        oss << " extends " << node->bases[0].baseName;
        if (node->bases.size() > 1) {
            oss << " /* Multiple inheritance: ";
            for (size_t i = 1; i < node->bases.size(); ++i) {
                oss << node->bases[i].access << " " << node->bases[i].baseName;
                if (i + 1 < node->bases.size()) oss << ", ";
            }
            oss << " (ignored) */";
        }
    }
    oss << " {\n";
    auto emitMembers = [&](const std::vector<std::unique_ptr<ASTNode>>& members) {
        for (const auto& member : members) {
            if (member->type == ASTNodeType::VAR_DECL) {
                oss << "    public " << generateVarDecl(static_cast<const VarDecl*>(member.get()), node->name) << "\n";
            } else if (member->type == ASTNodeType::FUNCTION_DECL) {
                const FunctionDecl* func = static_cast<const FunctionDecl*>(member.get());
                oss << "    public " << generateFunctionDecl(func, node->name, true) << "\n";
            }
        }
    };
    emitMembers(node->publicMembers);
    emitMembers(node->protectedMembers);
    emitMembers(node->privateMembers);
    oss << "}\n";
    return oss.str();
}

// ...existing code...
std::string JavaCodeGenerator::generateEnumDecl(const EnumDecl* node)  {
    std::ostringstream oss;
    oss << "public enum " << node->name << " { ";
    for (size_t i = 0; i < node->enumerators.size(); ++i) {
        // If enumerator is a pair<string, int> or similar:
        oss << node->enumerators[i].first;
        if (i + 1 < node->enumerators.size()) oss << ", ";
    }
    oss << " }\n";
    return oss.str();
}





std::string JavaCodeGenerator::generateFunctionCall(const FunctionCall* node, const std::string& className)  {
    std::ostringstream oss;
    oss << generate(node->callee.get(), className) << "(";
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        oss << generate(node->arguments[i].get(), className);
        if (i + 1 < node->arguments.size()) oss << ", ";
    }
    oss << ")";
    return oss.str();
}

// --- For Statement ---
std::string JavaCodeGenerator::generateForStmt(const ForStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "for (";
    // --- Init ---
    if (node->init) {
        std::string initStr = generate(node->init.get(), className);
        // Remove trailing whitespace
        while (!initStr.empty() && std::isspace(static_cast<unsigned char>(initStr.back())))
            initStr.pop_back();
        // Remove a single trailing semicolon if present
        while (!initStr.empty() && initStr.back() == ';')
            initStr.pop_back();
        
        oss << initStr;
    }
    oss << "; ";
    // --- Condition ---
    if (node->condition) {
        std::string condStr = generate(node->condition.get(), className);
        while (!condStr.empty() && std::isspace(static_cast<unsigned char>(condStr.back())))
            condStr.pop_back();
        // Remove a single trailing semicolon if present
        while (!condStr.empty() && condStr.back() == ';')
            condStr.pop_back();
        oss << condStr;
    }
    oss << "; ";
    // --- Increment ---
    if (node->increment) {
        std::string incStr = generate(node->increment.get(), className);
        while (!incStr.empty() && std::isspace(static_cast<unsigned char>(incStr.back())))
            incStr.pop_back();
        // Remove a single trailing semicolon if present
        while (!incStr.empty() && incStr.back() == ';')
            incStr.pop_back();
        oss << incStr;
    }
    oss << ") ";
    
    if (node->body) oss << "{\n" << generate(node->body.get(), className) << "}";
    else oss << "{}";
    return oss.str();
}


std::string JavaCodeGenerator::generateWhileStmt(const WhileStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "while (" << generate(node->condition.get(), className) << ") ";
    if (node->body) oss << "{\n" << generate(node->body.get(), className) << "}";
    else oss << "{}";
    return oss.str();
}

std::string JavaCodeGenerator::generateDoWhileStmt(const DoWhileStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "do ";
    if (node->body) oss << "{\n" << generate(node->body.get(), className) << "}";
    else oss << "{}";
    oss << " while (" << generate(node->condition.get(), className) << ");";
    return oss.str();
}

std::string JavaCodeGenerator::generateBreakStmt(const BreakStmt*, const std::string&)  {
    return "break;";
}
std::string JavaCodeGenerator::generateContinueStmt(const ContinueStmt*, const std::string&)  {
    return "continue;";
}




// --- Preprocessor Directive ---
std::string JavaCodeGenerator::generatePreprocessorDirective(const PreprocessorDirective* node)  {
    std::ostringstream oss;
    oss << "// " << node->directiveText << "\n"; // Embed the exact C++ preprocessor directive as a comment
    return oss.str();
}



















// --- Try/Catch/Throw mapping ---
std::string JavaCodeGenerator::generateTryStmt(const TryStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "try " << generate(node->tryBlock.get(), className);
    for (const auto& catchNode : node->catchClauses) {
        const CatchStmt* catchBlock = static_cast<const CatchStmt*>(catchNode.get());
        std::string exType = "Exception";
        if (catchBlock->exceptionType) {
            // Try to get type name if possible
            if (catchBlock->exceptionType->type == ASTNodeType::QUALIFIED_TYPE) {
                exType = static_cast<const QualifiedType*>(catchBlock->exceptionType.get())->name;
            }
        }
        oss << " catch (" << exType << " " << catchBlock->exceptionVar << ") "
            << generate(catchBlock->body.get(), className);
    }
    return oss.str();
}

std::string JavaCodeGenerator::generateCatchStmt(const CatchStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "catch (" << mapTypeNodeToJava(node->exceptionType.get()) << " " << node->exceptionVar << ") "
        << generate(node->body.get(), className);
    return oss.str();
}
// --- Switch Statement ---
std::string JavaCodeGenerator::generateSwitchStmt(const SwitchStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "switch (" << generate(node->condition.get(), className) << ") {\n";
    for (const auto& stmt : node->cases) {
        oss << generate(stmt.get(), className) << "\n";
    }
    oss << "}";
    return oss.str();
}

std::string JavaCodeGenerator::generateCaseStmt(const CaseStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "case " << generate(node->value.get(), className) << ":\n";
    for (const auto& stmt : node->statements) {
        oss << generate(stmt.get(), className) << "\n";
    }
    // oss << "break;";
    return oss.str();
}

std::string JavaCodeGenerator::generateDefaultStmt(const DefaultStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "default:\n";
    for (const auto& stmt : node->statements) {
        oss << generate(stmt.get(), className) << "\n";
    }
    oss << "break;";
    return oss.str();
}

std::string JavaCodeGenerator::generateThrowStmt(const ThrowStmt* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "throw ";
    if (node->expression) {
        oss << generate(node->expression.get(), className);
    } else {
        oss << "new Exception()";
    }
    oss << ";";
    return oss.str();
}

std::string JavaCodeGenerator::generateSortCall(const SortCall* node, const std::string& className)  {
    // Map std::sort(vec.begin(), vec.end()) to Collections.sort(vec)
    requiredImports.insert("import java.util.Collections;");
    std::ostringstream oss;
    oss << "Collections.sort(" << generate(node->container.get(), className) << ");";
    return oss.str();
}

std::string JavaCodeGenerator::generateFindCall(const FindCall* node, const std::string& className)  {
    // Map std::find(container.begin(), container.end(), value) to container.contains(value)
    std::ostringstream oss;
    oss << generate(node->container.get(), className)
        << ".contains(" << generate(node->value.get(), className) << ")";
    return oss.str();
}

std::string JavaCodeGenerator::generateAccumulateCall(const AccumulateCall* node, const std::string& className)  {
    // Java does not have direct equivalents for C++ accumulate; emit a warning and a possible stream reduce
    std::ostringstream oss;
    oss << "// WARNING: std::accumulate not directly mappable; manual reduction needed\n";
    oss << generate(node->beginExpr.get(), className)
        << ".stream().reduce(" << generate(node->initialValue.get(), className) << ", Integer::sum)";
    return oss.str();
}


// --- Container Method Calls ---
std::string JavaCodeGenerator::generateMemberAccess(const MemberAccess* node, const std::string& className)  {
    // Try to map STL container methods to Java equivalents using stlMethodMap
    std::string object = generate(node->object.get(), className);
    std::string method = node->memberName;
    // Try to get type from symbol table
    const ASTNode* typeNode = nullptr;
    if (node->object->type == ASTNodeType::IDENTIFIER) {
        std::string varName = static_cast<const Identifier*>(node->object.get())->name;
        auto it = symbolTable.find(varName);
        if (it != symbolTable.end()) {
            typeNode = it->second;
        }
    }
    std::string javaType;
    if (typeNode && typeNode->type == ASTNodeType::TEMPLATE_TYPE) {
        const TemplateType* tt = static_cast<const TemplateType*>(typeNode);
        javaType = mapCppTypeNameToJava(tt->baseTypeName, false);
    }
    if (!javaType.empty() && stlMethodMap.count(javaType) && stlMethodMap.at(javaType).count(method)) {
        std::string javaMethod = stlMethodMap.at(javaType).at(method);
        logFile << "[JCG] Mapping member access: " << object << "." << method << "() to Java method: " << javaMethod << "()" << std::endl;
        return object + "." + javaMethod + "()";
    }
    // Fallback: emit as-is with a warning
    logFile << "[JCG][WARN] Unmapped member access: " << object << "." << method << "()" << std::endl;
    return "// WARNING: Unmapped member access: " + object + "." + method + "()";
}
std::string JavaCodeGenerator::generateArrayAccess(const ArrayAccess* node, const std::string& className)  {
    std::string base = generate(node->arrayExpr.get(), className);
    std::string index = generate(node->indexExpr.get(), className);

    // Try to get the type from the symbol table if arrayExpr is an Identifier
    const ASTNode* typeNode = nullptr;
    if (node->arrayExpr->type == ASTNodeType::IDENTIFIER) {
        std::string varName = static_cast<const Identifier*>(node->arrayExpr.get())->name;
        auto it = symbolTable.find(varName);
        if (it != symbolTable.end()) {
            typeNode = it->second;
        }
    }

    bool isMap = false;
    if (typeNode && typeNode->type == ASTNodeType::TEMPLATE_TYPE) {
        const TemplateType* tt = static_cast<const TemplateType*>(typeNode);
        std::string javaType = mapCppTypeNameToJava(tt->baseTypeName, false);
        if (javaType == "HashMap" || javaType == "Map") {
            isMap = true;
        }
    }

    if (isMap) {
        return base + ".get(" + index + ")";
    } else {
        return base + "[" + index + "]";
    }
}



// --- Lambda Expressions ---
std::string JavaCodeGenerator::generateLambdaExpr(const LambdaExpr* node, const std::string& className)  {
    // Map C++ lambda to Java lambda (if possible)
    // Only basic mapping, no captures
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        const VarDecl* param = static_cast<const VarDecl*>(node->parameters[i].get());
        oss << mapTypeNodeToJava(param->type.get()) << " " << param->name;
        if (i + 1 < node->parameters.size()) oss << ", ";
    }
    oss << ") -> ";
    if (node->body) {
        oss << generate(node->body.get(), className);
    } else {
        oss << "{}";
    }
    return oss.str();
}

// --- I/O Stream Expressions ---
std::string JavaCodeGenerator::generateCoutExpr(const CoutExpr* node, const std::string& className)  {
    // Map to System.out.print/println
    requiredImports.insert("import java.io.*;");
    std::ostringstream oss;
    oss << "System.out.println(";
    for (size_t i = 0; i < node->outputValues.size(); ++i) {
        oss << generate(node->outputValues[i].get(), className);
        if (i + 1 < node->outputValues.size()) oss << " + \" \" + ";
    }
    oss << ");";
    return oss.str();
}



std::string JavaCodeGenerator::generateCerrExpr(const CerrExpr* node, const std::string& className)  {
    requiredImports.insert("import java.io.*;");
    std::ostringstream oss;
    oss << "System.err.println(";
    for (size_t i = 0; i < node->errorOutputs.size(); ++i) {
        oss << generate(node->errorOutputs[i].get(), className);
        if (i + 1 < node->errorOutputs.size()) oss << " + \" \" + ";
    }
    oss << ");";
    return oss.str();
}

std::string JavaCodeGenerator::generateCinExpr(const CinExpr* node, const std::string& className)  {
    requiredImports.insert("import java.util.Scanner;");
    std::ostringstream oss;
    oss << "Scanner sc = new Scanner(System.in);\n";
    for (const auto& target : node->inputTargets) {
        oss << generate(target.get(), className) << " = sc.next();\n";
    }
    return oss.str();
}




std::string JavaCodeGenerator::generateVectorAccess(const VectorAccess* node, const std::string& className)  {
    // Map vector access to Java ArrayList access
    std::ostringstream oss;
    oss << generate(node->vectorExpr.get(), className) << "." << node->method << "(";
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        oss << generate(node->arguments[i].get(), className);
        if (i + 1 < node->arguments.size()) oss << ", ";
    }
    oss << ")";
    return oss.str();
}


std::string JavaCodeGenerator::generateGetlineCall(const GetlineCall* node, const std::string& className)  {
    // Map std::getline to Java Scanner nextLine
    requiredImports.insert("import java.util.Scanner;");
    std::ostringstream oss;
    oss << "Scanner sc = new Scanner(System.in);\n";
    oss << generate(node->targetVar.get(), className) << " = sc.nextLine();";
    return oss.str();
}

std::string JavaCodeGenerator::generatePrintfCall(const PrintfCall* node, const std::string& className)  {
    // Map printf to System.out.printf
    requiredImports.insert("import java.io.*;");
    std::ostringstream oss;
    oss << "System.out.printf(";
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        oss << generate(node->arguments[i].get(), className);
        if (i + 1 < node->arguments.size()) oss << ", ";
    }
    oss << ");";
    return oss.str();
}

std::string JavaCodeGenerator::generateScanfCall(const ScanfCall* node, const std::string& className)  {
    // Map scanf to Java Scanner usage
    requiredImports.insert("import java.util.Scanner;");
    std::ostringstream oss;
    oss << "Scanner sc = new Scanner(System.in);\n";
    for (const auto& target : node->inputTargets) {
        oss << generate(target.get(), className) << " = sc.next();\n";
    }
    return oss.str();
}


std::string JavaCodeGenerator::generateMallocCall(const MallocCall* node, const std::string& className)  {
    std::ostringstream oss;
    oss << "// WARNING: malloc is not applicable in Java (use new operator)\n";
    oss << "new " << mapTypeNodeToJava(node->elementType.get()) << "["
        << generate(node->sizeExpr.get(), className) << "]";
    return oss.str();
}

std::string JavaCodeGenerator::generateFreeCall(const FreeCall* /*node*/, const std::string& /*className*/)  {
    // Emit warning for free as it is not applicable in Java
    return "// WARNING: free is not applicable in Java (garbage collected)";
}

std::string JavaCodeGenerator::generateAbsCall(const AbsCall* node, const std::string& className) {
    requiredImports.insert("import java.lang.Math;");
    std::ostringstream oss;
    oss << "Math.abs(" << generate(node->valueExpr.get(), className) << ")";
    return oss.str();
}

std::string JavaCodeGenerator::generateSqrtCall(const MathFunctionCall* node, const std::string& className)  {
    // Assumes node->arguments.size() == 1
    requiredImports.insert("import java.lang.Math;");
    return "Math.sqrt(" + generate(node->arguments[0].get(), className) + ")";
}

std::string JavaCodeGenerator::generatePowCall(const MathFunctionCall* node, const std::string& className) {
    // Assumes node->arguments.size() == 2
    requiredImports.insert("import java.lang.Math;");
    return "Math.pow(" + generate(node->arguments[0].get(), className) + ", " + generate(node->arguments[1].get(), className) + ")";
}



std::string JavaCodeGenerator::generateTemplateClassDecl(const TemplateClassDecl* node, const std::string& className)  {
    // Java generics differ from C++ templates; emit a warning and best-effort mapping
    std::ostringstream oss;
    oss << "// WARNING: Template class not directly mappable to Java generics\n";
    oss << "public class " << node->name << "<";
    for (size_t i = 0; i < node->templateParams.size(); ++i) {
        oss << node->templateParams[i]->name;
        if (i + 1 < node->templateParams.size()) oss << ", ";
    }
    oss << "> {\n";
    for (const auto& member : node->members) {
        if (member->type == ASTNodeType::VAR_DECL)
            oss << "    " << generateVarDecl(static_cast<const VarDecl*>(member.get()), className) << "\n";
        else if (member->type == ASTNodeType::FUNCTION_DECL)
            oss << "    " << generateFunctionDecl(static_cast<const FunctionDecl*>(member.get()), className) << "\n";
    }
    oss << "}\n";
    return oss.str();
}
std::string JavaCodeGenerator::generateTemplateFunctionDecl(const TemplateFunctionDecl* node, const std::string& className) {
    // Java generics differ from C++ templates; emit a warning and best-effort mapping
    std::ostringstream oss;
    oss << "// WARNING: Template function not directly mappable to Java generics\n";
    oss << "public static <";
    for (size_t i = 0; i < node->templateParams.size(); ++i) {
        oss << node->templateParams[i]->name;
        if (i + 1 < node->templateParams.size()) oss << ", ";
    }
    oss << "> " << mapTypeNodeToJava(node->returnType.get()) << " " << node->name << "(";
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        const VarDecl* param = static_cast<const VarDecl*>(node->parameters[i].get());
        oss << mapTypeNodeToJava(param->type.get()) << " " << param->name;
        if (i + 1 < node->parameters.size()) oss << ", ";
    }
    oss << ")";
    if (node->body) {
        oss << "{\n" << generate(node->body.get(), className) << "}";
    } else {
        oss << " {}";
    }
    return oss.str();
}

std::string JavaCodeGenerator::generateInitializerListExpr(const InitializerListExpr* node, const std::string& className)  {
    // Map initializer list to Arrays.asList
    requiredImports.insert("import java.util.Arrays;");
    std::ostringstream oss;
    oss << "Arrays.asList(";
    for (size_t i = 0; i < node->elements.size(); ++i) {
        oss << generate(node->elements[i].get(), className);
        if (i + 1 < node->elements.size()) oss << ", ";
    }
    oss << ")";
    return oss.str();
}

std::string JavaCodeGenerator::generateThreadDecl(const ThreadDecl* node, const std::string& className)  {
    // Map thread declaration to Java Thread
    requiredImports.insert("import java.lang.Thread;");
    std::ostringstream oss;
    oss << "Thread " << node->threadVarName << " = new Thread(() -> " << generate(node->callable.get(), className) << ");";
    return oss.str();
}

std::string JavaCodeGenerator::generateMutexDecl(const MutexDecl* /*node*/, const std::string& /*className*/)  {
    // Map mutex declaration to Java synchronized block
    return "// WARNING: Mutex not directly mappable; use synchronized blocks in Java";
}

std::string JavaCodeGenerator::generateLockGuardDecl(const LockGuardDecl* /*node*/, const std::string& /*className*/)  {
    // Map lock_guard to Java synchronized block
    return "// WARNING: lock_guard not directly mappable; use synchronized blocks in Java";
}

std::string JavaCodeGenerator::generateFindIfCall(const FindCall* node, const std::string& className)  {
    // Map std::find_if to Java streams filter
    requiredImports.insert("import java.util.stream.*;");
    std::ostringstream oss;
    oss << generate(node->container.get(), className) << ".stream().filter(" << generate(node->value.get(), className) << ").findFirst().orElse(null);";
    return oss.str();
}

// --- New Expression ---
std::string JavaCodeGenerator::generateNewExpr(const NewExpr* node, const std::string& className)  {
    // Map C++ 'new Type[sz]' to Java 'new Type[sz]'
    if (node->type && node->type->type == ASTNodeType::POINTER_TYPE && !node->args.empty()) {
        std::string base = mapTypeNodeToJava(static_cast<const PointerType*>(node->type.get())->baseType.get());
        return "new " + base + "[" + generate(node->args[0].get(), className) + "]";
    }
    //can implement better logic
    std::ostringstream oss;
    oss << "new " << mapTypeNodeToJava(node->type.get());
    if (!node->args.empty()) {
        oss << "(";
        for (size_t i = 0; i < node->args.size(); ++i) {
            oss << generate(node->args[i].get(), className);
            if (i + 1 < node->args.size()) oss << ", ";
        }
        oss << ")";
    } else {
        oss << "()";
    }
    // For array new: new Type[size]
    // if (node->isArray && node->arraySize) {
    //     oss.str(""); // Clear previous
    //     oss << "new " << mapTypeNodeToJava(node->type.get()) << "[" << generate(node->arraySize.get(), className) << "]";
    // }
    return oss.str();
}

// --- Delete Expression ---
std::string JavaCodeGenerator::generateDeleteExpr(const DeleteExpr* node, const std::string& /*className*/)  {
    // In Java, delete is not needed (garbage collected)
    if (node->isArrayDelete) {
        return "// WARNING: delete[] is not applicable in Java (garbage collected)";
    } else {
        return "// WARNING: delete is not applicable in Java (garbage collected)";
    }
}

std::string JavaCodeGenerator::generateUnsupportedFeature(const std::string& feature, const std::string& details)  {
    std::ostringstream oss;
    oss << "// UNSUPPORTED: " << feature;
    if (!details.empty()) oss << ": " << details;
    oss << "\n";
    return oss.str();
}

// MathFunctionCall
std::string JavaCodeGenerator::generateMathFunctionCall(const MathFunctionCall* node, const std::string& className)  {
    const std::string& fname = node->functionName;
    // Defensive: check argument count for each function
    auto arg = [&](size_t i) -> std::string {
        if (i < node->arguments.size() && node->arguments[i])
            return generate(node->arguments[i].get(), className);
        return "/*missing_arg*/";
    };
    if (fname == "abs" || fname == "fabs") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.abs(" + arg(0) + ")";
    } else if (fname == "pow") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.pow(" + arg(0) + ", " + arg(1) + ")";
    } else if (fname == "sqrt") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.sqrt(" + arg(0) + ")";
    } else if (fname == "sin") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.sin(" + arg(0) + ")";
    } else if (fname == "cos") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.cos(" + arg(0) + ")";
    } else if (fname == "tan") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.tan(" + arg(0) + ")";
    } else if (fname == "floor") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.floor(" + arg(0) + ")";
    } else if (fname == "ceil") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.ceil(" + arg(0) + ")";
    } else if (fname == "round") {
        requiredImports.insert("import java.lang.Math;");
        return "Math.round(" + arg(0) + ")";
    } else if (fname == "rand") {
        requiredImports.insert("import java.util.Random;");
        return "(new java.util.Random()).nextInt()";
    } else if (fname == "srand") {
        return "// WARNING: srand not directly mappable in Java";
    } else if (fname == "strcmp") {
        return arg(0) + ".compareTo(" + arg(1) + ")";
    } else if (fname == "strncmp") {
        // Java: substring and compareTo, but check for missing args
        return arg(0) + ".substring(0, " + arg(2) + ").compareTo(" + arg(1) + ".substring(0, " + arg(2) + "))";
    } else if (fname == "strcpy") {
        // Java: assignment
        return arg(0) + " = " + arg(1);
    } else if (fname == "strncpy") {
        // Java: substring assignment
        return arg(0) + " = " + arg(1) + ".substring(0, " + arg(2) + ")";
    } else if (fname == "strlen") {
        return arg(0) + ".length()";
    } else if (fname == "strcat") {
        // Use StringBuilder for idiomatic Java
        return arg(0) + " = new StringBuilder(" + arg(0) + ").append(" + arg(1) + ").toString()";
    } else if (fname == "strncat") {
        // Use StringBuilder and substring for idiomatic Java
        return arg(0) + " = new StringBuilder(" + arg(0) + ").append(" + arg(1) + ".substring(0, " + arg(2) + ")).toString()";
    }
    // fallback
    return "// UNSUPPORTED: std function '" + fname + "'";
}


