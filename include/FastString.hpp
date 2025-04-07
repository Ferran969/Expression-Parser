#pragma once

#include <string>

// Temporary implementation for testing until a string repositiory is built.

namespace frontend::string {

class FastString {
public:
    inline FastString() = default;
    inline FastString(std::string&& data) : m_data(std::move(data)) {}
    inline FastString(const std::string& data) : m_data(data) {}
    inline FastString(const FastString& other) : m_data(other.m_data) {}
    inline bool operator<(const FastString& other) const noexcept { return m_data < other.m_data; }
    inline bool operator>(const FastString& other) const noexcept { return m_data > other.m_data; }
    inline bool operator==(const FastString& other) const noexcept { return m_data == other.m_data; }
    inline bool operator!=(const FastString& other) const noexcept { return m_data != other.m_data; }
    inline FastString& operator=(const FastString& other) { m_data = other.m_data; return *this; }
    inline const std::string& str() const noexcept { return m_data; }
private:
    std::string m_data;
};

} // namespace frontend::string