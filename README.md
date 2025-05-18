# Transpiler

## Features
- **Lexer:** Breaks down C++ source code into tokens (e.g., keywords, identifiers, operators).
- **Parser:** Constructs an AST from the tokens, representing the structure of the code.
- **AST Output:** Displays the parsed AST in a human-readable format.
- **Java Code Generation:** Converts a subset of C++ code (see below) to Java code.

Currently, it supports basic C++ constructs like function declarations, variable declarations, if statements, and return statements.

## Supported C++ Features
- Function declarations (no parameters)
- Variable declarations and initialization (one per line)
- If statements
- Return statements
- Simple binary expressions (e.g., `x > 0`, `x + 1`)

## Limitations
- **No support for preprocessor directives** (e.g., `#include <iostream>`)
- **No support for I/O** (`std::cout`, `std::cin`, stream operators)
- **No support for for-loops, while-loops, or do-while-loops**
- **No support for multiple variable declarations in one line** (e.g., `int a = 0, b = 1;`)
- **No support for increment/decrement operators** (`++`, `--`)
- **No support for function parameters or overloading**
- **No support for classes, structs, or advanced C++ features**
- Only simple types (`int`, `float`) are supported; others (e.g., `long long`) are not recognized

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
- `JavaCodeGenerator.hpp` / `JavaCodeGenerator.cpp`: Generates Java code from the AST.
- `test.cpp`: Sample C++ input file for testing the transpiler.

## Compile the Code
Run the following command to compile all source files into an executable named transpiler:

```sh
g++ Main.cpp lexer.cpp token.cpp parser.cpp JavaCodeGenerator.cpp -o transpiler
```

## Run the Transpiler
Execute the program with a C++ file as input:

```sh
./transpiler test.cpp
```

Or, to specify the output Java file name:

```sh
./transpiler test.cpp test.java
```

## View Output
The console will display the tokenization process, the resulting AST, and the generated Java code for the input file.

---

<<<<<<< Updated upstream
View Output: The console will display the tokenization process and the resulting AST for test.cpp.


Temp access Token: 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
=======
**Note:**
- The transpiler is a proof-of-concept and only supports a small subset of C++.
- For more complex C++ code, you will need to extend the parser, AST, and code generator.
>>>>>>> Stashed changes
