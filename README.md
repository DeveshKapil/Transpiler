# Transpiler

## Features
- **Lexer:** Breaks down C++ source code into tokens (e.g., keywords, identifiers, operators).
- **Parser:** Constructs an AST from the tokens, representing the structure of the code.
- **AST Output:** Displays the parsed AST in a human-readable format.

Currently, it supports basic C++ constructs like function declarations, variable declarations, if statements, and return statements.

## Prerequisites
To compile and run this project, you need:
- A C++ compiler (e.g., `g++` from MinGW on Windows, or any GCC-compatible compiler on Linux/Mac).
- A terminal or command-line interface (e.g., PowerShell on Windows, Bash on Linux/Mac).

## Files
- `Main.cpp`: Entry point of the program; reads the input file and coordinates lexing and parsing.
- `lexer.hpp` / `lexer.cpp`: Defines and implements the lexer to tokenize C++ code.
- `token.hpp` / `token.cpp`: Defines the token structure and its string representation.
- `parser.hpp` / `parser.cpp`: Defines and implements the parser to build the AST.
- `ast.hpp`: Defines the AST node types and their string representation.
- `test.cpp`: Sample C++ input file for testing the transpiler.

Compile the Code: Run the following command to compile all source files into an executable named transpiler:
powershell

g++ Main.cpp lexer.cpp token.cpp parser.cpp -o transpiler

Run the Transpiler: Execute the program with test.cpp as input:
powershell

./transpiler test.cpp

View Output: The console will display the tokenization process and the resulting AST for test.cpp.


Temp access Token: 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