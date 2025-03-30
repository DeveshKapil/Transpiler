#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <source_file> [output_file]\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open source file '" << argv[1] << "'\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();

    std::cout << "Starting program...\n";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    if (lexer.hasError()) {
        std::cerr << "Lexer error: " << lexer.getError() << "\n";
        return 1;
    }

    std::cout << "Tokens generated: " << tokens.size() << "\n";
    std::cout << "Before moving tokens: " << tokens.size() << "\n";
    Parser parser(std::move(tokens));
    std::cout << "After parser construction\n";
    try {
        auto ast = parser.parse();
        std::cout << "Parsing successful! AST:\n";
        std::cout << ast->toString() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Parser error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Program completed.\n";
    return 0;
}