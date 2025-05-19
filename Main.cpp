#include "lexer.hpp"
#include "parser.hpp"
#include "JavaCodeGenerator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include<string>

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

    std::string inputfile = argv[2];
    std::stringstream ss(inputfile);
    std::string token;
    std::getline(ss, token, '.');
    std::string outputfile = token;
    std::cout << "File Name: " << outputfile << std::endl;


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
        // Generate Java code from AST
        JavaCodeGenerator generator;
        std::string javaCode = generator.generate(ast.get() , outputfile);
        std::cout << "\nGenerated Java code:\n" << javaCode << std::endl;
        // Optionally write to output file if provided
        if (argc >= 3) {
            std::ofstream out(argv[2]);
            if (out.is_open()) {
                out << javaCode;
                out.close();
                std::cout << "Java code written to " << argv[2] << std::endl;
            } else {
                std::cerr << "Error: Could not open output file '" << argv[2] << "'\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Parser error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Program completed.\n";
    return 0;
}