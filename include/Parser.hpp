#pragma once

#include "ASTNode.hpp"
#include "Lexer.hpp"

namespace frontend {

class Parser final {
public:
    Parser(Lexer<2>&& lexer)
        : m_lexer(std::move(lexer))
    {
    }

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = default;
    Parser& operator=(Parser&&) = default;

    std::unique_ptr<Expression> parseExpression() { return parseEquality(); }

private:
    // Helper method, does not consume any delimiter.
    std::vector<std::unique_ptr<Expression>> parseList(TokenType);
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseRelational();
    std::unique_ptr<Expression> parseShift();
    std::unique_ptr<Expression> parseAdditive();
    std::unique_ptr<Expression> parseMultiplicative();
    std::unique_ptr<Expression> parseUnaryRightAssociative();
    std::unique_ptr<Expression> parseUnaryLeftAssociative();
    std::unique_ptr<Expression> parseTerminal();
    std::unique_ptr<Expression> parseArrayLiteral();
    Lexer<2> m_lexer;
};

}