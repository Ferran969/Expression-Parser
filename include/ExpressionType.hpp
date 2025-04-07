#pragma once

#include <memory>
#include <utility>

namespace frontend {

enum class BasicExpressionType {
    Bool,
    String,
    Uint8,
    Uint16,
    Uint24,
    Uint32,
    Uint40,
    Uint48,
    Uint56,
    Uint64,
    Uint72,
    Uint80,
    Uint88,
    Uint96,
    Uint104,
    Uint112,
    Uint120,
    Uint128,
    Uint136,
    Uint144,
    Uint152,
    Uint160,
    Uint168,
    Uint176,
    Uint184,
    Uint192,
    Uint200,
    Uint208,
    Uint216,
    Uint224,
    Uint232,
    Uint240,
    Uint248,
    Uint256,
    Int8,
    Int16,
    Int24,
    Int32,
    Int40,
    Int48,
    Int56,
    Int64,
    Int72,
    Int80,
    Int88,
    Int96,
    Int104,
    Int112,
    Int120,
    Int128,
    Int136,
    Int144,
    Int152,
    Int160,
    Int168,
    Int176,
    Int184,
    Int192,
    Int200,
    Int208,
    Int216,
    Int224,
    Int232,
    Int240,
    Int248,
    Int256,
    Array
};

class ExpressionType {
public:
    ExpressionType(BasicExpressionType type) : m_type(type) {}
    ~ExpressionType() = default;
    virtual bool operator==(const ExpressionType& other) const
    {
        return m_type == other.m_type;
    }
    virtual bool operator!=(const ExpressionType& other) const
    {
        return m_type != other.m_type;
    }
    inline BasicExpressionType basicExpressionType() const { return m_type; }
protected:
    BasicExpressionType m_type;
};

class ArrayType final : public ExpressionType {
public:
    ArrayType(std::unique_ptr<ExpressionType>&& elementType)
        : ExpressionType(BasicExpressionType::Array)
        , m_elementType(std::move(elementType))
    {
    }
    ~ArrayType() = default;
    bool operator==(const ExpressionType& other) const override
    {
        if (this->basicExpressionType() != other.basicExpressionType()) return false;
        auto& otherArray = static_cast<const ArrayType&>(other);
        return *m_elementType == *otherArray.m_elementType;
    }
    bool operator!=(const ExpressionType& other) const override
    {
        if (this->basicExpressionType() != other.basicExpressionType()) return true;
        auto& otherArray = static_cast<const ArrayType&>(other);
        return *m_elementType != *otherArray.m_elementType;
    }
private:
    std::unique_ptr<ExpressionType> m_elementType;
};

}