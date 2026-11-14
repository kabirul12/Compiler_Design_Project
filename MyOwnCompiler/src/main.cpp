#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "MyOwnCompiler - educational language interpreter\n"
                  << "Usage: myowncompiler <source-file.moc> [--tokens] [--symbols]\n";
        return 0;
    }

    const std::string filename = argv[1];
    bool showTokens = false, showSymbols = false;
    for (int i = 2; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "--tokens") showTokens = true;
        if (option == "--symbols") showSymbols = true;
    }

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "File Error: cannot open '" << filename << "'.\n";
        return 1;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    const std::string source = buffer.str();

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        if (showTokens) {
            std::cout << "--- TOKENS ---\n";
            for (const auto& token : tokens) {
                std::cout << token.line << ":" << token.column << "  "
                          << token.lexeme << '\n';
            }
        }

        Parser parser(tokens);
        parser.run();
        if (showSymbols) parser.printSymbols();
    } catch (const std::exception& e) {
        std::cerr << "Compiler Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
