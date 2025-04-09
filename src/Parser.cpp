#include "Parser.hpp"

#include <utility>

auto frontend::Parser::parseList(TokenType delimiter) -> std::vector<std::unique_ptr<Expression>>
{
    std::vector<std::unique_ptr<Expression>> list;
    // Check for immediate termination.
    if (m_lexer.peek().type == delimiter) {
        return list;
    }
    // There is, at least, one element.
    list.push_back(std::move(parseEquality()));
    auto opType = m_lexer.peek().type;
    while (opType == TokenType::Comma) {
        m_lexer.skip();
        list.push_back(std::move(parseEquality()));
        opType = m_lexer.peek().type;
    }
    // Should have reached delimiter, check should be done by the caller.
    return list;
}

auto frontend::Parser::parseEquality() -> std::unique_ptr<Expression>
{
    auto lhs = parseRelational();
    auto opType = m_lexer.peek().type;
    while (opType == TokenType::DoubleEquals || opType == TokenType::ExclamationEquals) {
        m_lexer.skip();
        auto rhs = parseRelational();
        if (opType == TokenType::DoubleEquals) {
            lhs = std::make_unique<Equals>(std::move(lhs), std::move(rhs));
        } else {
            lhs = std::make_unique<NotEquals>(std::move(lhs), std::move(rhs));
        }
        opType = m_lexer.peek().type;
    }
    return lhs;
}

auto frontend::Parser::parseRelational() -> std::unique_ptr<Expression>
{
    auto lhs = parseShift();
    auto opType = m_lexer.peek().type;
    while (
        opType == TokenType::LessThan ||
        opType == TokenType::LessEquals ||
        opType == TokenType::GreaterThan ||
        opType == TokenType::GreaterEquals
    ) {
        m_lexer.skip();
        auto rhs = parseShift();
        if (opType == TokenType::LessThan) {
            lhs = std::make_unique<LessThan>(std::move(lhs), std::move(rhs));
        } else if (opType == TokenType::LessEquals) {
            lhs = std::make_unique<LessEquals>(std::move(lhs), std::move(rhs));
        } else if (opType == TokenType::GreaterThan) {
            lhs = std::make_unique<GreaterThan>(std::move(lhs), std::move(rhs));
        } else {
            lhs = std::make_unique<GreaterEquals>(std::move(lhs), std::move(rhs));
        }
        opType = m_lexer.peek().type;
    }
    return lhs;
}

auto frontend::Parser::parseShift() -> std::unique_ptr<Expression>
{
    auto lhs = parseAdditive();
    auto opType = m_lexer.peek().type;
    while (opType == TokenType::DoubleLessThan || opType == TokenType::DoubleGreaterThan || opType == TokenType::TripleGreaterThan) {
        m_lexer.skip();
        auto rhs = parseAdditive();
        if (opType == TokenType::DoubleLessThan) {
            lhs = std::make_unique<ShiftLeft>(std::move(lhs), std::move(rhs));
        } else if (opType == TokenType::DoubleGreaterThan) {
            lhs = std::make_unique<ShiftRight>(std::move(lhs), std::move(rhs));
        } else {
            lhs = std::make_unique<ShiftRightLogic>(std::move(lhs), std::move(rhs));
        }
        opType = m_lexer.peek().type;
    }
    return lhs;
}

auto frontend::Parser::parseAdditive() -> std::unique_ptr<Expression>
{
    auto lhs = parseMultiplicative();
    auto opType = m_lexer.peek().type;
    while (opType == TokenType::Plus || opType == TokenType::Minus) {
        m_lexer.skip();
        auto rhs = parseMultiplicative();
        if (opType == TokenType::Plus) {
            lhs = std::make_unique<Addition>(std::move(lhs), std::move(rhs));
        } else {
            lhs = std::make_unique<Subtraction>(std::move(lhs), std::move(rhs));
        }
        opType = m_lexer.peek().type;
    }
    return lhs;
}

auto frontend::Parser::parseMultiplicative() -> std::unique_ptr<Expression>
{
    auto lhs = parseUnaryRightAssociative();
    auto opType = m_lexer.peek().type;
    while (opType == TokenType::Star || opType == TokenType::Slash || opType == TokenType::Percent) {
        m_lexer.skip();
        auto rhs = parseUnaryRightAssociative();
        if (opType == TokenType::Star) {
            lhs = std::make_unique<Multiplication>(std::move(lhs), std::move(rhs));
        } else if (opType == TokenType::Slash) {
            lhs = std::make_unique<Division>(std::move(lhs), std::move(rhs));
        } else {
            lhs = std::make_unique<Remainder>(std::move(lhs), std::move(rhs));
        }
        opType = m_lexer.peek().type;
    }
    return lhs;
}

auto frontend::Parser::parseUnaryRightAssociative() -> std::unique_ptr<Expression>
{
    auto opType = m_lexer.peek().type;
    switch (opType) {
        case TokenType::Increment: {
            std::size_t from = m_lexer.peek().from;
            m_lexer.skip();
            return std::make_unique<PreIncrement>(std::move(parseUnaryRightAssociative()), from);
        }
        case TokenType::Decrement: {
            std::size_t from = m_lexer.peek().from;
            m_lexer.skip();
            return std::make_unique<PreDecrement>(std::move(parseUnaryRightAssociative()), from);
        }
        case TokenType::Plus: {
            std::size_t from = m_lexer.peek().from;
            m_lexer.skip();
            return std::make_unique<Negative>(std::move(parseUnaryRightAssociative()), from);
        }
        case TokenType::Minus: {
            std::size_t from = m_lexer.peek().from;
            m_lexer.skip();
            return std::make_unique<Positive>(std::move(parseUnaryRightAssociative()), from);
        }
        case TokenType::Exclamation: {
            std::size_t from = m_lexer.peek().from;
            m_lexer.skip();
            return std::make_unique<LogicalNegation>(std::move(parseUnaryRightAssociative()), from);
        }
        case TokenType::Tilde: {
            std::size_t from = m_lexer.peek().from;
            m_lexer.skip();
            return std::make_unique<BitwiseNot>(std::move(parseUnaryRightAssociative()), from);
        }
        default:
            return parseUnaryLeftAssociative();
    }
}

auto frontend::Parser::parseUnaryLeftAssociative() -> std::unique_ptr<Expression>
{
    // By precondition, there should be some term.
    auto target = parseTerminal();
    auto opType = m_lexer.peek().type;
    // TODO: extend the supported operators.
    while (
        opType == TokenType::Dot ||
        opType == TokenType::OpenParenthesis ||
        opType == TokenType::OpenBracket ||
        opType == TokenType::Increment ||
        opType == TokenType::Decrement
    ) {
        switch (opType) {
            case TokenType::Dot: {
                // Skip '.';
                m_lexer.skip();
                if (m_lexer.peek().type != TokenType::Identifier) {
                    throw std::runtime_error(
                        "Expected identifier, but got '" +
                        m_lexer.peek().lexeme.str() +
                        "' from " + std::to_string(m_lexer.peek().from) +
                        " to " + std::to_string(m_lexer.peek().to) + "."
                    );
                }
                auto identifier = std::make_unique<Identifier>(m_lexer.peek().lexeme, m_lexer.peek().from, m_lexer.peek().to);
                target = std::make_unique<MemberAccess>(std::move(target), std::move(identifier));
                // Skip identifier.
                m_lexer.skip();
                break;
            }
            case TokenType::OpenParenthesis: {
                // Skip '(';
                m_lexer.skip();
                auto arguments = parseList(TokenType::CloseParenthesis);
                if (m_lexer.peek().type != TokenType::CloseParenthesis) {
                    throw std::runtime_error(
                        "Expected ')', but got '" +
                        m_lexer.peek().lexeme.str() +
                        "' from " + std::to_string(m_lexer.peek().from) +
                        " to " + std::to_string(m_lexer.peek().to) + "."
                    );
                }
                // Put the 'to' from ')'.
                target = std::make_unique<FunctionCall>(std::move(target), std::move(arguments), m_lexer.peek().to);
                // Skip ')'.
                m_lexer.skip();
                break;
            }
            case TokenType::OpenBracket: {
                // Skip '['.
                m_lexer.skip();
                auto subscript = parseExpression();
                if (m_lexer.peek().type != TokenType::CloseBracket) {
                    throw std::runtime_error(
                        "Expected ']', but got '" +
                        m_lexer.peek().lexeme.str() +
                        "' from " + std::to_string(m_lexer.peek().from) +
                        " to " + std::to_string(m_lexer.peek().to) + "."
                    );
                }
                // Put the 'to' from ']'.
                target = std::make_unique<SubscriptAccess>(std::move(target), std::move(subscript), m_lexer.peek().to);
                // Skip ']'.
                m_lexer.skip();
                break;
            }
            case TokenType::Increment: {
                // Put the 'to' from '++'.
                target = std::make_unique<PostIncrement>(std::move(target), m_lexer.peek().to);
                // Skip '++'.
                m_lexer.skip();
                break;
            }
            case TokenType::Decrement: {
                // Put the 'to' from '--'.
                target = std::make_unique<PostDecrement>(std::move(target), m_lexer.peek().to);
                // Skip '--'.
                m_lexer.skip();
                break;
            }
            // Should never reach default.
	    default: std::unreachable();
        }
        opType = m_lexer.peek().type;
    }
    return target;
}

auto frontend::Parser::parseTerminal() -> std::unique_ptr<Expression>
{
    auto type = m_lexer.peek().type;
    if (type == TokenType::Identifier) {
        auto node = std::make_unique<Identifier>(m_lexer.peek().lexeme, m_lexer.peek().from, m_lexer.peek().to);
        m_lexer.skip();
        return node;
    } else if (type == TokenType::NumericLiteral) {
        auto node = std::make_unique<NumericLiteral>(m_lexer.peek().lexeme, m_lexer.peek().from, m_lexer.peek().to);
        m_lexer.skip();
        return node;
    } else if (type == TokenType::StringLiteral) {
        auto node = std::make_unique<StringLiteral>(m_lexer.peek().lexeme, m_lexer.peek().from, m_lexer.peek().to);
        m_lexer.skip();
        return node;
    } else if (type == TokenType::OpenBracket) {
        return parseArrayLiteral();
    } else {
        throw std::runtime_error(
            "Expected terminal, but got '" +
            m_lexer.peek().lexeme.str() +
            "' from " + std::to_string(m_lexer.peek().from) +
            " to " + std::to_string(m_lexer.peek().to) + "."
        );
    }
}

auto frontend::Parser::parseArrayLiteral() -> std::unique_ptr<Expression>
{
    // Save the 'from' from '['.
    std::size_t from = m_lexer.peek().from;
    // By precondition, we have '['.
    m_lexer.skip();
    auto array = parseList(TokenType::CloseBracket);
    // Assert that the next token is a ']'.
    if (m_lexer.peek().type != TokenType::CloseBracket) {
        throw std::runtime_error(
            "Expected ']', but got '" +
            m_lexer.peek().lexeme.str() +
            "' from " + std::to_string(m_lexer.peek().from) +
            " to " + std::to_string(m_lexer.peek().to) + "."
        );
    }
    // Save the 'to' from ']'.
    std::size_t to = m_lexer.peek().to;
    // Skip ']'.
    m_lexer.skip();
    return std::make_unique<ArrayLiteral>(std::move(array), from, to);
}
