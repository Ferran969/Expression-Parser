#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "ExpressionType.hpp"
#include "FastString.hpp"

namespace frontend {

enum class NodeType {
    Identifier,
    NumericLiteral,
    StringLiteral,
    ArrayLiteral,
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Remainder,
    MemberAccess,
    FunctionCall,
    SubscriptAccess,
    PostIncrement,
    PostDecrement,
    PreIncrement,
    PreDecrement,
    Negative,
    Positive,
    LogicalNegation,
    BitwiseNot,
    ShiftLeft,
    ShiftRight,
    ShiftRightLogic,
    LessThan,
    LessEquals,
    GreaterThan,
    GreaterEquals,
    Equals,
    NotEquals
};

// Base class.
class ASTNode {
public:
    ASTNode(NodeType type, std::size_t from, std::size_t to) noexcept
        : m_nodeType(type), m_from(from), m_to(to) {}
    ASTNode(ASTNode&&) noexcept = default;
    ASTNode& operator=(ASTNode&&) noexcept = default;
    ASTNode(const ASTNode&) = delete;
    ASTNode& operator=(const ASTNode&) = delete;
    virtual ~ASTNode() = default;
    NodeType nodeType() const noexcept { return m_nodeType; }
    std::size_t from() const noexcept { return m_from; }
    std::size_t to() const noexcept { return m_to; }
    virtual void dump(std::ostream& os, std::size_t indent) const = 0;
    
protected:
    // Protected helper to print indentation.
    static void printIndent(std::ostream& os, std::size_t indent)
    {
        for (std::size_t i = 0; i < indent; ++i) {
            os << ' ';
        }
    }

    const NodeType m_nodeType;
    const std::size_t m_from;
    const std::size_t m_to;
};

class Expression : public ASTNode {
public:
    using ASTNode::ASTNode;
    virtual ~Expression() = default;
};

class Identifier final : public Expression {
public:
    Identifier(const string::FastString& identifier, std::size_t from, std::size_t to) noexcept
        : Expression(NodeType::Identifier, from, to)
        , m_identifier(identifier)
    {
    }
    ~Identifier() = default;
    void dump(std::ostream& os, std::size_t indent) const override
    {
        ASTNode::printIndent(os, indent);
        os << "Identifier" << std::endl;
        ASTNode::printIndent(os, indent);
        os << "Value: " << m_identifier.str() << std::endl;
    }
private:
    string::FastString m_identifier;
};

class NumericLiteral final : public Expression {
public:
    NumericLiteral(const string::FastString& literal, std::size_t from, std::size_t to) noexcept
        : Expression(NodeType::NumericLiteral, from, to)
        , m_literal(literal)
    {
    }
    ~NumericLiteral() = default;
    void dump(std::ostream& os, std::size_t indent) const override
    {
        ASTNode::printIndent(os, indent);
        os << "NumericLiteral" << std::endl;
        ASTNode::printIndent(os, indent);
        os << "Value: " << m_literal.str() << std::endl;
    }
private:
    string::FastString m_literal;
};

class StringLiteral final : public Expression {
public:
    StringLiteral(const string::FastString& literal, std::size_t from, std::size_t to) noexcept
        : Expression(NodeType::StringLiteral, from, to)
        , m_literal(literal)
    {
    }
    ~StringLiteral() = default;
    void dump(std::ostream& os, std::size_t indent) const override
    {
        ASTNode::printIndent(os, indent);
        os << "StringLiteral" << std::endl;
        ASTNode::printIndent(os, indent);
        os << "Value: " << m_literal.str() << std::endl;
    }
private:
    string::FastString m_literal;
};

class ArrayLiteral final : public Expression {
public:
    ArrayLiteral(std::vector<std::unique_ptr<Expression>>&& array, std::size_t from, std::size_t to) noexcept
        : Expression(NodeType::ArrayLiteral, from, to)
        , m_array(std::move(array))
    {
    }
    ~ArrayLiteral() = default;
    void dump(std::ostream& os, std::size_t indent) const override
    {
        ASTNode::printIndent(os, indent);
        os << "ArrayLiteral" << std::endl;
        ASTNode::printIndent(os, indent);
        os << " [" << std::endl;
        bool first = true;
        for (const auto& element : m_array) {
            if (!first) {
                os << std::endl;
            }
            first = false;
            element->dump(os, indent + 2);
        }
        ASTNode::printIndent(os, indent);
        os << " ]" << std::endl;
    }
private:
    std::vector<std::unique_ptr<Expression>> m_array;
};

class UnaryOperator : public Expression {
public:
    UnaryOperator(NodeType type, std::unique_ptr<Expression>&& argument, std::size_t from, std::size_t to) noexcept
        : Expression(type, from, to)
        , m_argument(std::move(argument))
    {
    }
    virtual ~UnaryOperator() = default;
protected:
    std::unique_ptr<Expression> m_argument;
};

class BinaryOperator : public Expression {
public:
    BinaryOperator(NodeType type, std::unique_ptr<Expression>&& lhs, std::unique_ptr<Expression>&& rhs) noexcept
        : Expression(type, lhs->from(), rhs->to())
        , m_lhs(std::move(lhs))
        , m_rhs(std::move(rhs))
    {
    }
    virtual ~BinaryOperator() = default;
protected:
    std::unique_ptr<Expression> m_lhs;
    std::unique_ptr<Expression> m_rhs;
};

#define DECLARE_BINARY_OPERATOR(CLASS_NAME)                                                         \
class CLASS_NAME final : public BinaryOperator {                                                    \
public:                                                                                             \
    CLASS_NAME(std::unique_ptr<Expression>&& lhs, std::unique_ptr<Expression>&& rhs) noexcept         \
        : BinaryOperator(NodeType::CLASS_NAME, std::move(lhs), std::move(rhs))                      \
    {                                                                                               \
    }                                                                                               \
    ~CLASS_NAME() = default;                                                                        \
    void dump(std::ostream& os, std::size_t indent) const override                                  \
    {                                                                                               \
        ASTNode::printIndent(os, indent);                                                           \
        os << #CLASS_NAME << std::endl;                                                             \
        m_lhs->dump(os, indent + 1);                                                                \
        m_rhs->dump(os, indent + 1);                                                                \
    }                                                                                               \
};

DECLARE_BINARY_OPERATOR(Addition)
DECLARE_BINARY_OPERATOR(Subtraction)
DECLARE_BINARY_OPERATOR(Multiplication)
DECLARE_BINARY_OPERATOR(Division)
DECLARE_BINARY_OPERATOR(Remainder)

// Special binary operator, because rhs must be an Identifier.
class MemberAccess final : public Expression {
public:
    MemberAccess(std::unique_ptr<Expression>&& argument, std::unique_ptr<Identifier>&& identifier) noexcept
        : Expression(NodeType::MemberAccess, argument->from(), identifier->to())
        , m_argument(std::move(argument))
        , m_identifier(std::move(identifier))
    {
    }
    ~MemberAccess() = default;
    void dump(std::ostream& os, std::size_t indent) const override
    {
        ASTNode::printIndent(os, indent);
        os << "MemberAccess" << std::endl;
        ASTNode::printIndent(os, indent);
        os << " Argument" << std::endl;
        m_argument->dump(os, indent + 2);
        m_identifier->dump(os, indent + 1);
    }
private:
    std::unique_ptr<Expression> m_argument;
    std::unique_ptr<Identifier> m_identifier;
};

// Special case, only 'to' has to be explicitly provided.
class FunctionCall final : public Expression {
public:
    FunctionCall(std::unique_ptr<Expression>&& function, std::vector<std::unique_ptr<Expression>>&& arguments, std::size_t to) noexcept
        : Expression(NodeType::FunctionCall, function->from(), to)
        , m_function(std::move(function))
        , m_arguments(std::move(arguments))
    {
    }
    ~FunctionCall() = default;
    void dump(std::ostream& os, std::size_t indent) const override
    {
        ASTNode::printIndent(os, indent);
        os << "FunctionCall" << std::endl;
        ASTNode::printIndent(os, indent);
        os << " Function" << std::endl;
        m_function->dump(os, indent + 2);
        ASTNode::printIndent(os, indent);
        os << " Arguments" << std::endl;
        for (const auto& argument : m_arguments) {
            argument->dump(os, indent + 2);
        }
    }
private:
    std::unique_ptr<Expression> m_function;
    std::vector<std::unique_ptr<Expression>> m_arguments;
};

// Special case, only 'to' has to be explicitly provided.
class SubscriptAccess final : public Expression {
public:
    SubscriptAccess(std::unique_ptr<Expression>&& argument, std::unique_ptr<Expression>&& subscript, std::size_t to) noexcept
        : Expression(NodeType::SubscriptAccess, argument->from(), to)
        , m_argument(std::move(argument))
        , m_subscript(std::move(subscript))
    {
    }
    ~SubscriptAccess() = default;
    void dump(std::ostream& os, std::size_t indent) const override
    {
        ASTNode::printIndent(os, indent);
        os << "SubscriptAccess" << std::endl;
        ASTNode::printIndent(os, indent);
        os << " Argument" << std::endl;
        m_argument->dump(os, indent + 2);
        ASTNode::printIndent(os, indent);
        os << " Subscript" << std::endl;
        m_subscript->dump(os, indent + 2);
    }
private:
    std::unique_ptr<Expression> m_argument;
    std::unique_ptr<Expression> m_subscript;
};

#define DECLARE_RIGHT_UNARY_OPERATOR(CLASS_NAME)                                                    \
class CLASS_NAME final : public UnaryOperator {                                                     \
public:                                                                                             \
    CLASS_NAME(std::unique_ptr<Expression>&& argument, std::size_t to) noexcept                      \
        : UnaryOperator(NodeType::CLASS_NAME, std::move(argument), argument->from(), to)            \
    {                                                                                               \
    }                                                                                               \
    ~CLASS_NAME() = default;                                                                        \
    void dump(std::ostream& os, std::size_t indent) const override                                  \
    {                                                                                               \
        ASTNode::printIndent(os, indent);                                                           \
        os << #CLASS_NAME << std::endl;                                                             \
        m_argument->dump(os, indent + 1);                                                           \
    }                                                                                               \
};

DECLARE_RIGHT_UNARY_OPERATOR(PostIncrement)
DECLARE_RIGHT_UNARY_OPERATOR(PostDecrement)

#define DECLARE_LEFT_UNARY_OPERATOR(CLASS_NAME)                                                     \
class CLASS_NAME final : public UnaryOperator {                                                     \
public:                                                                                             \
    CLASS_NAME(std::unique_ptr<Expression>&& argument, std::size_t from) noexcept                    \
        : UnaryOperator(NodeType::CLASS_NAME, std::move(argument), from, argument->to())            \
    {                                                                                               \
    }                                                                                               \
    ~CLASS_NAME() = default;                                                                        \
    void dump(std::ostream& os, std::size_t indent) const override                                  \
    {                                                                                               \
        ASTNode::printIndent(os, indent);                                                           \
        os << #CLASS_NAME << std::endl;                                                             \
        m_argument->dump(os, indent + 1);                                                           \
    }                                                                                               \
};

DECLARE_LEFT_UNARY_OPERATOR(PreIncrement)
DECLARE_LEFT_UNARY_OPERATOR(PreDecrement)
DECLARE_LEFT_UNARY_OPERATOR(Positive)
DECLARE_LEFT_UNARY_OPERATOR(Negative)
DECLARE_LEFT_UNARY_OPERATOR(LogicalNegation)
DECLARE_LEFT_UNARY_OPERATOR(BitwiseNot)

DECLARE_BINARY_OPERATOR(ShiftLeft)
DECLARE_BINARY_OPERATOR(ShiftRight)
DECLARE_BINARY_OPERATOR(ShiftRightLogic)
DECLARE_BINARY_OPERATOR(LessThan)
DECLARE_BINARY_OPERATOR(LessEquals)
DECLARE_BINARY_OPERATOR(GreaterThan)
DECLARE_BINARY_OPERATOR(GreaterEquals)
DECLARE_BINARY_OPERATOR(Equals)
DECLARE_BINARY_OPERATOR(NotEquals)

#undef DECLARE_BINARY_OPERATOR
#undef DECLARE_RIGHT_UNARY_OPERATOR
#undef DECLARE_LEFT_UNARY_OPERATOR

} // namespace frontend