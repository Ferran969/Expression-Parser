#include "Lexer.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <stdlib.h>

void Usage()
{
    std::cerr << "Usage: ./testLexer <file>" << std::endl;
    exit(1);
}

// Only some tokens for basic testing.
std::string tokenTypeToString(frontend::TokenType type)
{
    switch (type) {
        case frontend::TokenType::EndOfFile: return "EndOfFile";
        case frontend::TokenType::NumericLiteral: return "NumericLiteral";
        case frontend::TokenType::StringLiteral: return "StringLiteral";
        case frontend::TokenType::Plus: return "Plus";
        case frontend::TokenType::Minus: return "Minus";
        case frontend::TokenType::Star: return "Star";
        case frontend::TokenType::Slash: return "Slash";
        default: throw std::logic_error("Unimplemented string for TokenType.");
    }
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
        frontend::Lexer<1> lexer(std::move(file));
        while (lexer.peek().type != frontend::TokenType::EndOfFile) {
            auto& token = lexer.peek();
            std::cout << "Token type: " << tokenTypeToString(token.type) << std::endl;
            std::cout << "Lexeme: " << token.lexeme.str() << std::endl;
            std::cout << "From: " << token.from << std::endl;
            std::cout << "To: " << token.to << std::endl;
            std::cout << std::endl;
            lexer.skip();
        }
    } catch (std::exception& exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
        return 1;
    }
    return 0;
}