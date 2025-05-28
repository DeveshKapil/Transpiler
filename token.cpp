#include "tokens.hpp"
#include <sstream>
#include <iomanip>
#include <unordered_map>

namespace {

static const std::unordered_map<TokenType, const char*> token_type_names = {
    {TokenType::END_OF_FILE, "END_OF_FILE"}, {TokenType::ERROR, "ERROR"},
    {TokenType::IDENTIFIER, "IDENTIFIER"}, {TokenType::INTEGER, "INTEGER"},
    {TokenType::FLOAT, "FLOAT"}, {TokenType::STRING, "STRING"},
    {TokenType::CHARACTER, "CHARACTER"}, {TokenType::INT, "INT"},
    {TokenType::VOID, "VOID"}, {TokenType::CHAR, "CHAR"},
    {TokenType::FLOAT_TYPE, "FLOAT_TYPE"}, {TokenType::DOUBLE, "DOUBLE"},
    {TokenType::BOOL, "BOOL"}, {TokenType::CLASS, "CLASS"},
    {TokenType::STRUCT, "STRUCT"}, {TokenType::ENUM, "ENUM"},
    {TokenType::UNION, "UNION"}, {TokenType::CONST, "CONST"},
    {TokenType::UNSIGNED, "UNSIGNED"}, {TokenType::SIGNED, "SIGNED"},
    {TokenType::SHORT, "SHORT"}, {TokenType::LONG, "LONG"},
    {TokenType::STATIC, "STATIC"}, {TokenType::EXTERN, "EXTERN"},
    {TokenType::REGISTER, "REGISTER"}, {TokenType::INLINE, "INLINE"},
    {TokenType::VIRTUAL, "VIRTUAL"}, {TokenType::EXPLICIT, "EXPLICIT"},
    {TokenType::FRIEND, "FRIEND"}, {TokenType::PRIVATE, "PRIVATE"},
    {TokenType::PUBLIC, "PUBLIC"}, {TokenType::PROTECTED, "PROTECTED"},
    {TokenType::IF, "IF"}, {TokenType::ELSE, "ELSE"},
    {TokenType::FOR, "FOR"}, {TokenType::WHILE, "WHILE"},
    {TokenType::DO, "DO"}, {TokenType::SWITCH, "SWITCH"},
    {TokenType::CASE, "CASE"}, {TokenType::DEFAULT, "DEFAULT"},
    {TokenType::BREAK, "BREAK"}, {TokenType::CONTINUE, "CONTINUE"},
    {TokenType::RETURN, "RETURN"}, {TokenType::GOTO, "GOTO"},
    {TokenType::NEWLINE, "NEWLINE"}, 

    {TokenType::PLUS, "PLUS"}, {TokenType::MINUS, "MINUS"},
    {TokenType::STAR, "STAR"}, {TokenType::SLASH, "SLASH"},
    {TokenType::PERCENT, "PERCENT"}, {TokenType::AMPERSAND, "AMPERSAND"},
    {TokenType::PIPE, "PIPE"}, {TokenType::CARET, "CARET"},
    {TokenType::TILDE, "TILDE"}, {TokenType::EXCLAIM, "EXCLAIM"},
    {TokenType::EQUAL, "EQUAL"}, {TokenType::LESS, "LESS"},
    {TokenType::GREATER, "GREATER"}, {TokenType::PLUS_EQUAL, "PLUS_EQUAL"},
    {TokenType::MINUS_EQUAL, "MINUS_EQUAL"}, {TokenType::STAR_EQUAL, "STAR_EQUAL"},
    {TokenType::SLASH_EQUAL, "SLASH_EQUAL"}, {TokenType::PERCENT_EQUAL, "PERCENT_EQUAL"},
    {TokenType::AND_EQUAL, "AND_EQUAL"}, {TokenType::OR_EQUAL, "OR_EQUAL"},
    {TokenType::XOR_EQUAL, "XOR_EQUAL"}, {TokenType::LESS_LESS, "LESS_LESS"},
    {TokenType::GREATER_GREATER, "GREATER_GREATER"},

    // New multi-char assignment operators
    {TokenType::LEFT_SHIFT_EQUAL, "LEFT_SHIFT_EQUAL"},
    {TokenType::RIGHT_SHIFT_EQUAL, "RIGHT_SHIFT_EQUAL"},

    {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
    {TokenType::NOT_EQUAL, "NOT_EQUAL"},
    {TokenType::LESS_EQUAL, "LESS_EQUAL"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
    {TokenType::AND_AND, "AND_AND"},
    {TokenType::OR_OR, "OR_OR"},
    {TokenType::DECREMENT, "DECREMENT"},
    {TokenType::ARROW, "ARROW"},
    {TokenType::INCREMENT, "INCREMENT"},
   

    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
    {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::DOT, "DOT"},
    {TokenType::COLON, "COLON"},
    {TokenType::SCOPE, "SCOPE"},
    {TokenType::HASH, "HASH"},
    {TokenType::QUESTION, "QUESTION"},

    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::CHAR_LITERAL, "CHAR_LITERAL"},

    // STL Containers
    {TokenType::VECTOR, "VECTOR"},
    {TokenType::MAP, "MAP"},
    {TokenType::SET, "SET"},
    {TokenType::LIST, "LIST"},
    {TokenType::DEQUE, "DEQUE"},
    {TokenType::UNORDERED_MAP, "UNORDERED_MAP"},
    {TokenType::UNORDERED_SET, "UNORDERED_SET"},
    {TokenType::STRING_LIB, "STRING_LIB"},
    {TokenType::MULTIMAP, "MULTIMAP"},
    {TokenType::MULTISET, "MULTISET"},
    {TokenType::STACK, "STACK"},
    {TokenType::QUEUE, "QUEUE"},
    {TokenType::PRIORITY_QUEUE, "PRIORITY_QUEUE"},
    {TokenType::BITSET, "BITSET"},
    {TokenType::ARRAY, "ARRAY"},
    {TokenType::FORWARD_LIST, "FORWARD_LIST"},
    {TokenType::PAIR, "PAIR"},
    {TokenType::TUPLE, "TUPLE"},
    {TokenType::OPTIONAL, "OPTIONAL"},
    {TokenType::VARIANT, "VARIANT"},
    {TokenType::ANY, "ANY"},
    {TokenType::SPAN, "SPAN"},
    {TokenType::VALARRAY, "VALARRAY"},


    // C standard lib functions
    {TokenType::PRINTF, "PRINTF"},
    {TokenType::SCANF, "SCANF"},
    {TokenType::MALLOC, "MALLOC"},
    {TokenType::FREE, "FREE"},
    {TokenType::MEMCPY, "MEMCPY"},
    {TokenType::COUT, "COUT"},
    {TokenType::CIN, "CIN"},
    {TokenType::CERR, "CERR"},
    {TokenType::CLIN, "CLIN"},  
    {TokenType::GETLINE, "GETLINE"},
    

    // Template and Namespace tokens
    {TokenType::TEMPLATE, "TEMPLATE"},
    {TokenType::NAMESPACE, "NAMESPACE"},
    {TokenType::USING, "USING"},
    {TokenType::TYPEDEF, "TYPEDEF"},
    {TokenType::TEMPLATE_LESS, "TEMPLATE_LESS"},
    {TokenType::TEMPLATE_GREATER, "TEMPLATE_GREATER"},
    {TokenType::TEMPLATE_COMMA, "TEMPLATE_COMMA"},

    // Preprocessor directives
    {TokenType::PREPROCESSOR_INCLUDE, "PREPROCESSOR_INCLUDE"},
    {TokenType::PREPROCESSOR_DEFINE, "PREPROCESSOR_DEFINE"},
    {TokenType::PREPROCESSOR_UNDEF, "PREPROCESSOR_UNDEF"},
    {TokenType::PREPROCESSOR_PRAGMA, "PREPROCESSOR_PRAGMA"},
    {TokenType::PREPROCESSOR_UNKNOWN, "PREPROCESSOR_UNKNOWN"},
    {TokenType::PREPROCESSOR_IFDEF, "PREPROCESSOR_IFDEF"},
    {TokenType::PREPROCESSOR_IFNDEF, "PREPROCESSOR_IFNDEF"},
    {TokenType::PREPROCESSOR_IF, "PREPROCESSOR_IF"},
    {TokenType::PREPROCESSOR_ELIF, "PREPROCESSOR_ELIF"},
    {TokenType::PREPROCESSOR_ELSE, "PREPROCESSOR_ELSE"},
    {TokenType::PREPROCESSOR_ENDIF, "PREPROCESSOR_ENDIF"},

    {TokenType::NEW, "NEW"},
    {TokenType::DELETE, "DELETE"},

    {TokenType::ABS, "ABS"},
    {TokenType::FABS, "FABS"},
    {TokenType::POW, "POW"},   
    {TokenType::SQRT, "SQRT"},
    {TokenType::SIN, "SIN"},
    {TokenType::COS, "COS"},
    {TokenType::TAN, "TAN"},
    {TokenType::FLOOR, "FLOOR"},
    {TokenType::CEIL, "CEIL"},
    {TokenType::ROUND, "ROUND"},
    {TokenType::RAND, "RAND"},
    {TokenType::SRAND, "SRAND"},

    {TokenType::EXIT, "EXIT"},

    {TokenType::STOI, "STOI"},
    {TokenType::STOF, "STOF"},
    {TokenType::STOD, "STOD"},
    {TokenType::TO_STRING, "TO_STRING"},
    {TokenType::STRCMP, "STRCMP"},
    {TokenType::STRNCMP, "STRNCMP"},
    {TokenType::STRCHR, "STRCHR"},
    {TokenType::STRRCHR, "STRRCHR"},
    {TokenType::STRSTR, "STRSTR"},
    {TokenType::STRCAT, "STRCAT"},
    {TokenType::STRNCAT, "STRNCAT"},
    {TokenType::STRCPY, "STRCPY"},
    {TokenType::STRLEN, "STRLEN"},

    {TokenType::ALLOCATE, "ALLOCATE"},
    {TokenType::DEALLOCATE, "DEALLOCATE"},

    {TokenType::TIME, "TIME"},
    {TokenType::CLOCK, "CLOCK"},
    {TokenType::DIFFTIME, "DIFFTIME"},
    {TokenType::STRFTIME, "STRFTIME"},
    {TokenType::LOCALTIME, "LOCALTIME"},
    {TokenType::GMTIME, "GMTIME"},

    {TokenType::THREAD, "THREAD"},
    {TokenType::MUTEX, "MUTEX"},
    {TokenType::LOCK_GUARD, "LOCK_GUARD"},
    {TokenType::UNIQUE_LOCK, "UNIQUE_LOCK"},
    {TokenType::ASYNC, "ASYNC"},
    {TokenType::FUTURE, "FUTURE"},
    {TokenType::PROMISE, "PROMISE"},

    {TokenType::TRY, "TRY"},
    {TokenType::CATCH, "CATCH"},
    {TokenType::THROW, "THROW"},
    {TokenType::EXCEPTION, "EXCEPTION"},
    {TokenType::LOGIC_ERROR, "LOGIC_ERROR"},
    {TokenType::RUNTIME_ERROR, "RUNTIME_ERROR"},
    {TokenType::TYPEID, "TYPEID"},
    {TokenType::STATIC_CAST, "STATIC_CAST"},
    {TokenType::DYNAMIC_CAST, "DYNAMIC_CAST"},
    {TokenType::CONST_CAST, "CONST_CAST"},
    {TokenType::REINTERPRET_CAST, "REINTERPRET_CAST"},
    {TokenType::TEMPLATE_LESS, "TEMPLATE_LESS"},
    {TokenType::TEMPLATE_GREATER, "TEMPLATE_GREATER"},
    {TokenType::TEMPLATE_COMMA, "TEMPLATE_COMMA"},
    // {TokenType::TEMPLATE_FUNCTION_DECL, "TEMPLATE_FUNCTION_DECL"},
    // {TokenType::TEMPLATE_PARAM, "TEMPLATE_PARAM"},
    // {TokenType::TEMPLATE_ARG, "TEMPLATE_ARG"},
    // {TokenType::TYPE, "TYPE"},
    // {TokenType::LAMBDA_EXPR, "LAMBDA_EXPR"},
    // {TokenType::STATIC_CAST_EXPR, "STATIC_CAST_EXPR"},
    // {TokenType::DYNAMIC_CAST_EXPR, "DYNAMIC_CAST_EXPR"},
    // {TokenType::CONST_CAST_EXPR, "CONST_CAST_EXPR"},
    // {TokenType::REINTERPRET_CAST_EXPR, "REINTERPRET_CAST_EXPR"},
    // {TokenType::TYPEID_EXPR, "TYPEID_EXPR"},
    // {TokenType::STREAM_EXPR, "STREAM_EXPR"},
    // {TokenType::PREPROCESSOR_DIRECTIVE, "PREPROCESSOR_DIRECTIVE"},
    {TokenType::PREPROCESSOR_INCLUDE, "PREPROCESSOR_INCLUDE"},
    {TokenType::PREPROCESSOR_DEFINE, "PREPROCESSOR_DEFINE"},
    {TokenType::PREPROCESSOR_IFDEF, "PREPROCESSOR_IFDEF"},
    {TokenType::PREPROCESSOR_IFNDEF, "PREPROCESSOR_IFNDEF"},
    {TokenType::PREPROCESSOR_IF, "PREPROCESSOR_IF"},
    {TokenType::PREPROCESSOR_ELIF, "PREPROCESSOR_ELIF"},
    {TokenType::PREPROCESSOR_ELSE, "PREPROCESSOR_ELSE"},
    {TokenType::PREPROCESSOR_ENDIF, "PREPROCESSOR_ENDIF"},  
    {TokenType::PREPROCESSOR_UNDEF, "PREPROCESSOR_UNDEF"},
    {TokenType::PREPROCESSOR_PRAGMA, "PREPROCESSOR_PRAGMA"},
    // {TokenType::NAMESPACE_DECL, "NAMESPACE_DECL"},
    // {TokenType::USING_DIRECTIVE, "USING_DIRECTIVE"},
    // {TokenType::CATCH_STMT, "CATCH_STMT"},
    // {TokenType::SQRT_CALL, "SQRT_CALL"},
    // {TokenType::POW_CALL, "POW_CALL"},
    // {TokenType::PROGRAM, "PROGRAM"},
    // {TokenType::NEW_EXPR, "NEW_EXPR"},
    // {TokenType::DELETE_EXPR, "DELETE_EXPR"},
    // {TokenType::INITIALIZER_LIST_EXPR, "INITIALIZER_LIST_EXPR"},
    // {TokenType::MALLOC_CALL, "MALLOC_CALL"},
    // {TokenType::FREE_CALL, "FREE_CALL"},
    // {TokenType::NEW_EXPR, "NEW_EXPR"},
    // {TokenType::DELETE_EXPR, "DELETE_EXPR"},
    // {TokenType::INITIALIZER_LIST_EXPR, "INITIALIZER_LIST_EXPR"},
    // {TokenType::NAMESPACE_DECL, "NAMESPACE_DECL"},
    // {TokenType::USING_DIRECTIVE, "USING_DIRECTIVE"},
    // {TokenType::THREAD_DECL, "THREAD_DECL"},
    // {TokenType::MUTEX_DECL, "MUTEX_DECL"},
    // {TokenType::LOCK_GUARD_DECL, "LOCK_GUARD_DECL"},
    // {TokenType::UNIQUE_LOCK_DECL, "UNIQUE_LOCK_DECL"},
    // {TokenType::ASYNC_EXPR, "ASYNC_EXPR"},
    // {TokenType::FUTURE_EXPR, "FUTURE_EXPR"},
    // {TokenType::PROMISE_DECL, "PROMISE_DECL"},
    // {TokenType::EXCEPTION_CLASS, "EXCEPTION_CLASS"},
    // {TokenType::LOGIC_ERROR_CLASS, "LOGIC_ERROR_CLASS"},
    // {TokenType::RUNTIME_ERROR_CLASS, "RUNTIME_ERROR_CLASS"},
    // {TokenType::VECTOR_TYPE, "VECTOR_TYPE"},
    // {TokenType::MAP_TYPE, "MAP_TYPE"},
    // {TokenType::SET_TYPE, "SET_TYPE"},
    // {TokenType::LIST_TYPE, "LIST_TYPE"},
    // {TokenType::DEQUE_TYPE, "DEQUE_TYPE"},
    // {TokenType::UNORDERED_MAP_TYPE, "UNORDERED_MAP_TYPE"},
    // {TokenType::UNORDERED_SET_TYPE, "UNORDERED_SET_TYPE"},
    // {TokenType::MULTIMAP_TYPE, "MULTIMAP_TYPE"},
    // {TokenType::MULTISET_TYPE, "MULTISET_TYPE"},
    // {TokenType::STACK_TYPE, "STACK_TYPE"},
    // {TokenType::QUEUE_TYPE, "QUEUE_TYPE"},
    // {TokenType::PRIORITY_QUEUE_TYPE, "PRIORITY_QUEUE_TYPE"},
    // {TokenType::BITSET_TYPE, "BITSET_TYPE"},



    
};

const char* getTokenTypeName(TokenType type) {
    auto it = token_type_names.find(type);
    return it != token_type_names.end() ? it->second : "UNKNOWN";
}

} // unnamed namespace

Token::Token() = default;
std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(type=" << getTokenTypeName(type())
        << ", text='" << text() << "'"
        << ", line=" << line()
        << ", column=" << column();

    if (type() == TokenType::INTEGER) {
        oss << ", int_value=" << int_value();
    } else if (type() == TokenType::FLOAT) {
        oss << ", float_value=" << std::fixed << std::setprecision(6) << float_value();
    }

    oss << ")";
    return oss.str();
}
