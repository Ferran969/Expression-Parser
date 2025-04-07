#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cctype>
#include <utility>

#include "CircularQueue.hpp"
#include "Token.hpp"

namespace frontend {

template <std::size_t numberOfLookaheads>
class Lexer final {
public:
    Lexer(std::ifstream&& file)
        : m_file(std::move(file))
        , m_index(0)
    {
        for (std::size_t i = 0; i < numberOfLookaheads; ++i) {
            addNextToken();
        }
    }

    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer(Lexer&&) = default;
    Lexer& operator=(Lexer&&) = default;

    const Token& peek() { return m_tokens.front(); }

    const Token& lookahead(std::size_t index) { return m_tokens[index]; }

    void skip() { m_tokens.pop(); addNextToken(); }

private:
    char peekChar() { return m_file.peek(); }

    void skipChar() { ++m_index; m_file.ignore(); }

    // Returns true if a slash has been found.
    bool skipWhitespace()
    {
        // Skips all whitespace.
        while (isspace(peekChar())) skipChar();
        // Skip comments.
        // Check potential comment.
        if (peekChar() == '/') {
            // Skip to make lookahead.
            skipChar();
            if (peekChar() == '/') {
                // Ignore '/' and the next characters untill
                // end-of-line or end-of-file.
                do {
                    skipChar();
                } while (peekChar() != '\n' && peekChar() != EOF);
                // Skip the following whitespaces recursively.
                skipWhitespace();
            } else if (peekChar() == '*') {
                // Ingore star.
                skipChar();
                bool terminated = false;
                while (peekChar() != EOF) {
                    if (peekChar() == '*') {
                        skipChar();
                        if (peekChar() == '/') {
                            skipChar();
                            terminated = true;
                            break;
                        }
                    } else {
                        skipChar();
                    }
                }
                if (!terminated) {
                    throw std::range_error("Unterminated multiline comment.");
                }
                // Skip the following whitespaces recursively.
                skipWhitespace();
            } else {
                // It was not a comment, it is a slash.
                m_tokens.push({ string::FastString("/"), TokenType::Slash, m_index - 1, m_index });
                return true;
            }
        }
        return false;
    }
        
    void addNextToken()
    {
        // Slash found.
        if (skipWhitespace()) return;
        std::size_t from = m_index;
        if (peekChar() == EOF) {
            m_tokens.push({ string::FastString(""), TokenType::EndOfFile, from, m_index });
            return;
        }
        switch (peekChar()) {
            case '[': {
                skipChar();
                m_tokens.push({ string::FastString("["), TokenType::OpenBracket, from, m_index });
                break;
            }
            case ']': {
                skipChar();
                m_tokens.push({ string::FastString("]"), TokenType::CloseBracket, from, m_index });
                break;
            }
            case '(': {
                skipChar();
                m_tokens.push({ string::FastString("("), TokenType::OpenParenthesis, from, m_index });
                break;
            }
            case ')': {
                skipChar();
                m_tokens.push({ string::FastString(")"), TokenType::CloseParenthesis, from, m_index });
                break;
            }
            case '.': {
                skipChar();
                m_tokens.push({ string::FastString("."), TokenType::Dot, from, m_index });
                break;
            }
            case ',': {
                skipChar();
                m_tokens.push({ string::FastString(","), TokenType::Comma, from, m_index });
                break;
            }
            case '+': {
                skipChar();
                if (peekChar() == '+') {
                    skipChar();
                    m_tokens.push({ string::FastString("++"), TokenType::Increment, from, m_index });
                } else {
                    m_tokens.push({ string::FastString("+"), TokenType::Plus, from, m_index });
                }
                break;
            }
            case '-': {
                skipChar();
                if (peekChar() == '-') {
                    skipChar();
                    m_tokens.push({ string::FastString("--"), TokenType::Decrement, from, m_index });
                } else {
                    m_tokens.push({ string::FastString("-"), TokenType::Minus, from, m_index });
                }
                break;
            }
            case '*': {
                skipChar();
                m_tokens.push({ string::FastString("*"), TokenType::Star, from, m_index });
                break;
            }
            case '/': {
                skipChar();
                m_tokens.push({ string::FastString("/"), TokenType::Slash, from, m_index });
                break;
            }
            case '%': {
                skipChar();
                m_tokens.push({ string::FastString("%"), TokenType::Percent, from, m_index });
                break;
            }
            case '!': {
                skipChar();
                if (peekChar() == '=') {
                    skipChar();
                    m_tokens.push({ string::FastString("!="), TokenType::ExclamationEquals, from, m_index });
                } else {
                    m_tokens.push({ string::FastString("!"), TokenType::Exclamation, from, m_index });
                }
                break;
            }
            case '~': {
                skipChar();
                m_tokens.push({ string::FastString("~"), TokenType::Tilde, from, m_index });
                break;
            }
            case '<': {
                skipChar();
                if (peekChar() == '<') {
                    skipChar();
                    m_tokens.push({ string::FastString("<<"), TokenType::DoubleLessThan, from, m_index });
                } else if (peekChar() == '=') {
                    skipChar();
                    m_tokens.push({ string::FastString("<="), TokenType::LessEquals, from, m_index });
                } else {
                    m_tokens.push({ string::FastString("<"), TokenType::LessThan, from, m_index });
                }
                break;
            }
            case '>': {
                skipChar();
                if (peekChar() == '>') {
                    skipChar();
                    if (peekChar() == '>') {
                        skipChar();
                        m_tokens.push({ string::FastString(">>>"), TokenType::TripleGreaterThan, from, m_index });
                    } else {
                        m_tokens.push({ string::FastString(">>"), TokenType::DoubleGreaterThan, from, m_index });
                    }
                } else if (peekChar() == '=') {
                    skipChar();
                    m_tokens.push({ string::FastString(">="), TokenType::GreaterEquals, from, m_index });
                } else {
                    m_tokens.push({ string::FastString(">"), TokenType::GreaterThan, from, m_index });
                }
                break;
            }
            case '=': {
                skipChar();
                if (peekChar() == '=') {
                    skipChar();
                    m_tokens.push({ string::FastString("=="), TokenType::DoubleEquals, from, m_index });
                } else {
                    m_tokens.push({ string::FastString("="), TokenType::Equals, from, m_index });
                }
                break;
            }
            case '"': {
                m_tokens.push({ getStringLiteral(), TokenType::StringLiteral, from, m_index });
                break;
            }
            default: {
                if (isdigit(peekChar())) {
                    m_tokens.push({ getNumericLiteral(), TokenType::NumericLiteral, from, m_index });
                } else if (isalpha(peekChar()) || peekChar() == '_') {
                    // TODO: check for keyword.
                    m_tokens.push({ getIdentifier(), TokenType::Identifier, from, m_index });
                } else {
                    throw std::logic_error("Unrecognized token.");
                }
                break;
            }
        }
    }

    frontend::string::FastString getNumericLiteral()
    {
        std::string literal;
        if (peekChar() == '0') {
            literal.push_back(peekChar());
            skipChar();
            if (peekChar() == 'b') {
                // Binary.
                literal.push_back(peekChar());
                skipChar();
                while (peekChar() == '0' || peekChar() == '1') {
                    literal.push_back(peekChar());
                    skipChar();
                }
                if (literal.size() == 2) {
                    // Check if there was nothing after the prefix.
                    throw std::range_error("Binary prefix without a number.");
                }
            } else if (peekChar() == 'o') {
                literal.push_back(peekChar());
                skipChar();
                while (peekChar() >= '0' && peekChar() <= '7') {
                    literal.push_back(peekChar());
                    skipChar();
                }
                if (literal.size() == 2) {
                    // Check if there was nothing after the prefix.
                    throw std::range_error("Octal prefix without a number.");
                }
            } else if (peekChar() == 'x') {
                literal.push_back(peekChar());
                skipChar();
                while (isdigit(peekChar()) || (toupper(peekChar()) >= 'A' && toupper(peekChar()) <= 'F')) {
                    literal.push_back(peekChar());
                    skipChar();
                }
                if (literal.size() == 2) {
                    // Check if there was nothing after the prefix.
                    throw std::range_error("Hexadecimal prefix without a number.");
                }
            } else if (isdigit(peekChar())) {
                throw std::range_error("Decimal numbers cannot start with 0 (use \"0o\" prefix for octal base).");
            }
            // Otherwise, it is number 0.
        } else {
            // Decimal.
            do {
                literal.push_back(peekChar());
                skipChar();
            } while (isdigit(peekChar()));
        }
        return string::FastString(std::move(literal));
    }
    
    frontend::string::FastString getStringLiteral()
    {
        std::string literal;
        // Ingore '"'.
        skipChar();
        while (peekChar() != '"' && peekChar() != '\n' && peekChar() != EOF) {
            literal.push_back(peekChar());
            skipChar();
        }
        if (peekChar() == '\n') {
            throw std::range_error("Reached end-of-line before ending string literal.");
        } else if (peekChar() == EOF) {
            throw std::range_error("Reached end-of-file before ending string literal.");
        }
        // Ingore '"'.
        skipChar();
        return string::FastString(std::move(literal));
    }

    frontend::string::FastString getIdentifier()
    {
        std::string identifier;
        // By precondition, it should be valid.
        identifier.push_back(peekChar());
        skipChar();
        while (isalnum(peekChar()) || peekChar() == '_') {
            identifier.push_back(peekChar());
            skipChar();
        }
        return string::FastString(std::move(identifier));
    }

    std::ifstream m_file;
    std::size_t m_index;
    CircularQueue<Token, numberOfLookaheads> m_tokens;
};

} // namespace frontend