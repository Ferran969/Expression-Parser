#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <utility>

#include "Lexer.hpp"
#include "Parser.hpp"

void Usage()
{
    std::cerr << "Usage: ./testParser <file>" << std::endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) Usage();
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file \"" << argv[1] << '"' << std::endl;
        return 1;
    }
    try {
        frontend::Lexer<2> lexer(std::move(file));
        frontend::Parser parser(std::move(lexer));
        auto ast = parser.parseExpression();
        ast->dump(std::cout, 0);
    } catch (std::exception& exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
        return 1;
    }
    return 0;
}