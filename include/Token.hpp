#pragma once

#include "FastString.hpp"

namespace frontend {

enum class TokenType : std::size_t {
    EndOfFile,
    Identifier,
    NumericLiteral,
    StringLiteral,
    OpenBracket,
    CloseBracket,
    OpenParenthesis,
    CloseParenthesis,
    Dot,
    Comma,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Increment,
    Decrement,
    Exclamation,
    Tilde,
    LessThan,
    LessEquals,
    GreaterThan,
    GreaterEquals,
    DoubleLessThan,
    DoubleGreaterThan,
    TripleGreaterThan,
    Equals,
    DoubleEquals,
    ExclamationEquals
};

struct Token {
    string::FastString lexeme;
    TokenType type;
    std::size_t from;
    std::size_t to;
};

} // namespace frontend