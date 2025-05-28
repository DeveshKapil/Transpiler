#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <memory>

enum class TokenType {
    END_OF_FILE, ERROR,

    IDENTIFIER, INTEGER, FLOAT, STRING, CHARACTER,

    // C++ keywords
    INT, VOID, CHAR, FLOAT_TYPE, DOUBLE, BOOL,
    CLASS, STRUCT, ENUM, UNION,
    CONST, UNSIGNED, SIGNED, SHORT, LONG,
    STATIC, EXTERN, REGISTER, INLINE,
    VIRTUAL, EXPLICIT, FRIEND,
    PRIVATE, PUBLIC, PROTECTED,
    IF, ELSE, FOR, WHILE, DO, SWITCH, CASE, DEFAULT, BREAK, CONTINUE, RETURN, GOTO,NEWLINE,

    // Operators & punctuation
    PLUS, MINUS, STAR, SLASH, PERCENT, AMPERSAND, PIPE, CARET, TILDE, EXCLAIM,
    EQUAL, LESS, GREATER, INCREMENT, DECREMENT,

    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,
    AND_EQUAL, OR_EQUAL, XOR_EQUAL,

    LESS_LESS, GREATER_GREATER,
    LEFT_SHIFT_EQUAL,    // <<=
    RIGHT_SHIFT_EQUAL,   // >>=

    EQUAL_EQUAL, NOT_EQUAL,
    LESS_EQUAL, GREATER_EQUAL,

    AND_AND, OR_OR,

    // PLUS_PLUS, MINUS_MINUS,
    QUESTION,     // ?
   


    ARROW,              // ->

    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    SEMICOLON, COMMA, DOT, COLON,
    SCOPE,      // ::

    HASH,       // #

    STRING_LITERAL,    // For string literals like "hello"
    CHAR_LITERAL,      // For char literals like 'a'

    // STL Containers
    VECTOR, MAP, SET, LIST, DEQUE,
    UNORDERED_MAP, UNORDERED_SET,
    MULTIMAP, MULTISET,
    STACK, QUEUE, PRIORITY_QUEUE,
    BITSET, ARRAY, FORWARD_LIST,
    PAIR, TUPLE, STRING_LIB,
    OPTIONAL, VARIANT, ANY,
    SPAN, VALARRAY,


    // C standard lib functions
    PRINTF, SCANF, MALLOC, FREE,
    MEMCPY, STRCPY, STRLEN,

    // C++ I/O Streams
    CIN, COUT, CERR, CLIN,
    GETLINE, PUT, GET, FLUSH,
    OPEN, CLOSE, READ, WRITE,

    // Algorithms
    SORT, FIND, COUNT, COPY,
    REVERSE, ACCUMULATE,
    ALL_OF, ANY_OF, NONE_OF,
    LOWER_BOUND, UPPER_BOUND,

    // Math
    ABS, FABS, POW, SQRT,
    SIN, COS, TAN,
    FLOOR, CEIL, ROUND,
    RAND, SRAND, EXIT,

    // Strings
    STOI, STOF, STOD, TO_STRING,
    STRCMP, STRNCMP,
    STRCHR, STRRCHR,
    STRSTR, STRCAT, STRNCAT,

    // Memory and allocation
    NEW, DELETE,
    ALLOCATE, DEALLOCATE,

    // Time
    TIME, CLOCK, DIFFTIME,
    STRFTIME, LOCALTIME, GMTIME,

    // Concurrency
    THREAD, MUTEX,
    LOCK_GUARD, UNIQUE_LOCK,
    ASYNC, FUTURE, PROMISE,

    // Exceptions
    TRY, CATCH, THROW,
    EXCEPTION, LOGIC_ERROR, RUNTIME_ERROR,

    // RTTI / Casting
    TYPEID, STATIC_CAST,
    DYNAMIC_CAST, CONST_CAST, REINTERPRET_CAST,

    TEMPLATE,   // template keyword token here

    // Namespace and template tokens
    TYPEDEF,
    USING,
    NAMESPACE,          // e.g., std
    TEMPLATE_LESS,      // <
    TEMPLATE_GREATER,   // >
    TEMPLATE_COMMA,     // ,

    // Preprocessor directives
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
    PREPROCESSOR_UNKNOWN

};


class Token {
public:
    
    Token();
    Token(TokenType type, std::string text, int line, int column)
        : type_(type), text_(std::move(text)), line_(line), column_(column),
          int_value_(0), is_float_(false), has_escape_(false) {}

    TokenType type() const { return type_; }
    const std::string& text() const { return text_; }
    int line() const { return line_; }
    int column() const { return column_; }
    long long int_value() const { return int_value_; }
    double float_value() const { return float_value_; }
    bool is_float() const { return is_float_; }
    bool has_escape() const { return has_escape_; }

    void set_int_value(long long value) { int_value_ = value; is_float_ = false; }
    void set_float_value(double value) { float_value_ = value; is_float_ = true; }

    std::string toString() const;

private:
    TokenType type_;
    std::string text_;
    int line_;
    int column_;
    union {
        long long int_value_;
        double float_value_;
    };
    bool is_float_;
    bool has_escape_;
};

#endif

