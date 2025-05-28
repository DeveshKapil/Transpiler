#include "lexer.hpp"
#include "parser.hpp"
#include "JavaCodeGenerator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
std::string getBaseName(const std::string& path) {
    // Find last slash or backslash (Windows or Unix paths)
    size_t lastSlash = path.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

    // Find last dot to remove extension
    size_t dot = filename.find_last_of('.');
    if (dot != std::string::npos) {
        return filename.substr(0, dot);
    }
    return filename; // No extension found
}
// ...existing code...

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open source file '" << argv[1] << "'\n";
        return 1;
    }

    std::string inputFilePath = argv[1];
    std::string baseName = getBaseName(inputFilePath);
    std::string javaOutputPath = "OUTPUT/" + baseName + ".java";

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();

    std::cout << "Running transpiler on source file: " << argv[1] << "\n\n";

    try {
        // Lexing
        Lexer lexer(source);
        std::vector<std::unique_ptr<Token>> tokens = lexer.tokenize();

        {
            std::ofstream lexerOut("OUTPUT/lexer_output.txt", std::ios::trunc);
            if (!lexerOut) {
                std::cerr << "Error: Could not open OUTPUT/lexer_output.txt for writing\n";
                return 1;
            }
            for (const auto& token : tokens) {
                lexerOut << token->toString() << std::endl;
            }
        }

        // Parsing
        Parser parser(std::move(tokens));
        std::unique_ptr<Program> ast = parser.parse();

        // {
        //     std::ofstream logFile("OUTPUT/parser_logs.txt", std::ios::trunc);
        //     if (!logFile) {
        //         std::cerr << "Error: Could not open OUTPUT/parser_logs.txt for writing\n";
        //         return 1;
        //     }
        //     for (const auto& log : parser.getParseLogs()) {
        //         logFile << log << std::endl;
        //     }
        // }

        std::cout << "\n--- AST Output ---\n";
        if (ast) {
            // Write AST output to file instead of terminal
            std::ofstream astOut("OUTPUT/ast_output.txt", std::ios::trunc);
            if (!astOut) {
                std::cerr << "Error: Could not open OUTPUT/ast_output.txt for writing\n";
                return 1;
            }
            astOut << ast->toString() << std::endl;
            astOut.flush();
            astOut.close();
        } else {
            std::cerr << "Error: AST is null. Aborting code generation.\n";
            return 1;
        }

        // Java Code Generation
        std::cout << "\n About to generate Java code...\n";
        JavaCodeGenerator codegen;
        std::string javaCode = codegen.generateProgram(ast.get(), baseName);
        std::cout << "Java code generation complete.\n";

        std::cout << "\n--- Generated Java Code ---\n";
        std::cout << javaCode << std::endl;

        {
            std::ofstream javaOut(javaOutputPath, std::ios::trunc);
            if (!javaOut) {
                std::cerr << "Error: Could not open " << javaOutputPath << " for writing\n";
                return 1;
            }
            javaOut << javaCode;
            javaOut.flush();
        }

        // {
        //     std::ofstream jcgLogOut("OUTPUT/JCG_logs.txt", std::ios::trunc);
        //     if (!jcgLogOut) {
        //         std::cerr << "Error: Could not open OUTPUT/JCG_logs.txt for writing\n";
        //         return 1;
        //     }
        //     for (const auto& log : codegen.JCG_logs) {
        //         jcgLogOut << log << std::endl;
        //     }
        //     jcgLogOut.flush();
        // }
    } 
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    std::cout << "\nTranspiler run completed.\n";
    return 0;
}
