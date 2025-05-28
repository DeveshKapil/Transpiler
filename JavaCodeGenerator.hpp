#ifndef JAVA_CODE_GENERATOR_HPP
#define JAVA_CODE_GENERATOR_HPP

#include <string>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <fstream>
#include "ast.hpp"

class JavaCodeGenerator {
public:
    std::string generateProgram(const Program* node, const std::string& className = "Main");
    mutable std::unordered_map<std::string, const ASTNode*> symbolTable;
    mutable std::set<std::string> requiredImports;
    std::set<std::string> userDefinedTemplates;
    mutable std::vector<std::string> JCG_logs;
    JavaCodeGenerator();
    ~JavaCodeGenerator();

private:
    std::ofstream logFile;

    std::string generate(const ASTNode* node, const std::string& className );
    // Main generators for top-level constructs
    std::string generateFunctionDecl(const FunctionDecl* node, const std::string& className, bool isClassMethod = false);
    std::string generateVarDecl(const VarDecl* node, const std::string& className);
    std::string generateBlockStmt(const BlockStmt* node, const std::string& className);
    std::string generateIfStmt(const IfStmt* node, const std::string& className);
    std::string generateReturnStmt(const ReturnStmt* node, const std::string& className);

    // Expressions
    std::string generateBinaryExpr(const BinaryExpr* node, const std::string& className);
    std::string generateAssignmentExpr(const AssignmentExpr* node, const std::string& className) ;
    std::string generateLiteral(const Literal* node);
    std::string generateIdentifier(const Identifier* node);

    // Type mapping
    std::string mapTypeNodeToJava(const ASTNode* typeNode, bool forGeneric = false) const;
    std::string mapCppTypeNameToJava(const std::string& cppType, bool forGeneric = false) const;    

    std::string generateClassDecl(const ClassDecl* node, const std::string& className);
    std::string generateStructDecl(const StructDecl* node, const std::string& className);
    std::string generateEnumDecl(const EnumDecl* node);

    std::string generateForStmt(const ForStmt* node, const std::string& className);
    std::string generateWhileStmt(const WhileStmt* node, const std::string& className);
    std::string generateDoWhileStmt(const DoWhileStmt* node, const std::string& className);
    std::string generateBreakStmt(const BreakStmt* node, const std::string& className);
    std::string generateContinueStmt(const ContinueStmt* node, const std::string& className);
    std::string generateExpressionStmt(const ExpressionStmt* node, const std::string& className);

    std::string generateUnaryExpr(const UnaryExpr* node, const std::string& className);
    std::string generateTernaryExpr(const TernaryExpr* node, const std::string& className);
    std::string generateFunctionCall(const FunctionCall* node, const std::string& className);
    std::string generateMemberAccess(const MemberAccess* node, const std::string& className);
    std::string generateArrayAccess(const ArrayAccess* node, const std::string& className);
    std::string generateSwitchStmt(const SwitchStmt* node, const std::string& className);
    std::string generateCaseStmt(const CaseStmt* node, const std::string& className);
    std::string generateDefaultStmt(const DefaultStmt* node, const std::string& className);
    std::string generateSortCall(const SortCall* node, const std::string& className);
    std::string generateFindCall(const FindCall* node, const std::string& className);
    std::string generateAccumulateCall(const AccumulateCall* node, const std::string& className);
    std::string generateVectorAccess(const VectorAccess* node, const std::string& className);
    std::string generateCoutExpr(const CoutExpr* node, const std::string& className);
    std::string generateCerrExpr(const CerrExpr* node, const std::string& className);
    std::string generateCinExpr(const CinExpr* node, const std::string& className);
    std::string generateGetlineCall(const GetlineCall* node, const std::string& className);
    std::string generatePrintfCall(const PrintfCall* node, const std::string& className);
    std::string generateScanfCall(const ScanfCall* node, const std::string& className);
    std::string generateMallocCall(const MallocCall* node, const std::string& className);
    std::string generateFreeCall(const FreeCall* node, const std::string& className);
    std::string generateAbsCall(const AbsCall* node, const std::string& className);
    std::string generateTemplateClassDecl(const TemplateClassDecl* node, const std::string& className);
    std::string generateTemplateFunctionDecl(const TemplateFunctionDecl* node, const std::string& className);
    std::string generateInitializerListExpr(const InitializerListExpr* node, const std::string& className);
    std::string generateThreadDecl(const ThreadDecl* node, const std::string& className);
    std::string generateMutexDecl(const MutexDecl* node, const std::string& className);
    std::string generateLockGuardDecl(const LockGuardDecl* node, const std::string& className);
    std::string generateFindIfCall(const FindCall* node, const std::string& className);
    std::string generateLambdaExpr(const LambdaExpr* node, const std::string& className);

    // Exception and memory handling
    std::string generateTryStmt(const TryStmt* node, const std::string& className);
    std::string generateThrowStmt(const ThrowStmt* node, const std::string& className);
    std::string generateNewExpr(const NewExpr* node, const std::string& className);
    std::string generateDeleteExpr(const DeleteExpr* node, const std::string& className);
    std::string generateUnsupportedFeature(const std::string& feature, const std::string& details);
    // std::string generateProgram(const Program* node, const std::string& className) const;
    std::string generatePreprocessorDirective(const PreprocessorDirective* node);
    std::string generateNamespaceDecl(const NamespaceDecl* node, const std::string& className);
    std::string generateUsingDirective(const UsingDirective* node, const std::string& className);
    std::string generateCatchStmt(const CatchStmt* node, const std::string& className);
    std::string generateSqrtCall(const MathFunctionCall* node, const std::string& className);
    std::string generatePowCall(const MathFunctionCall* node, const std::string& className);
    std::string generateMathFunctionCall(const MathFunctionCall* node, const std::string& className) ;
};

#endif
