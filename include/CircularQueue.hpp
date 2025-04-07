#pragma once

#include <cstddef>
#include <utility>
#include <new>
#include <stdexcept>

namespace frontend
{

template <typename T, std::size_t n>
class CircularQueue
{
public:
    CircularQueue() noexcept
        : m_buffer(static_cast<T*>(::operator new(n * sizeof(T))))
        , m_front(0)
        , m_back(n - 1)
        , m_size(0)
    {
    }

    // Copy constructor: deep copy by pushing constructed elements.
    CircularQueue(const CircularQueue& other)
        : m_buffer(static_cast<T*>(::operator new(n * sizeof(T))))
        , m_front(0)
        , m_back(n - 1)
        , m_size(0)
    {
        for (std::size_t i = 0; i < other.m_size; ++i) {
            push(other[i]);
        }
    }

    // Copy assignment operator: clear current contents and deep copy using push().
    CircularQueue& operator=(const CircularQueue& other)
    {
        if (this != &other) {
            clear();
            // Reset indices to start fresh.
            m_front = 0;
            m_back  = n - 1;
            m_size  = 0;
            for (std::size_t i = 0; i < other.m_size; ++i) {
                push(other[i]);
            }
        }
        return *this;
    }

    // Move constructor.
    CircularQueue(CircularQueue&& other) noexcept
        : m_buffer(other.m_buffer)
        , m_front(other.m_front)
        , m_back(other.m_back)
        , m_size(other.m_size)
    {
        other.m_buffer = nullptr;
        other.m_size = 0;
    }

    // Move assignment operator.
    CircularQueue& operator=(CircularQueue&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            ::operator delete(m_buffer);
            m_buffer = other.m_buffer;
            m_front = other.m_front;
            m_back = other.m_back;
            m_size = other.m_size;
            other.m_buffer = nullptr;
            other.m_size = 0;
        }
        return *this;
    }

    ~CircularQueue()
    {
        clear();
        ::operator delete(m_buffer);
    }

    std::size_t size() const noexcept
    {
        return m_size;
    }

    bool empty() const noexcept
    {
        return m_size == 0;
    }

    bool full() const noexcept
    {
        return m_size == n;
    }

    T& front()
    {
        if (m_size == 0) {
            throw std::underflow_error("CircularQueue: trying to access front in an empty queue");
        }
        return m_buffer[m_front];
    }

    const T& front() const
    {
        if (m_size == 0) {
            throw std::underflow_error("CircularQueue: trying to access front in an empty queue");
        }
        return m_buffer[m_front];
    }

    T& back()
    {
        if (m_size == 0) {
            throw std::underflow_error("CircularQueue: trying to access back in an empty queue");
        }
        return m_buffer[m_back];
    }

    const T& back() const
    {
        if (m_size == 0) {
            throw std::underflow_error("CircularQueue: trying to access back in an empty queue");
        }
        return m_buffer[m_back];
    }

    void push(const T& element)
    {
        if (m_size == n) {
            throw std::overflow_error("CircularQueue: size limit exceeded");
        }
        m_back = (m_back + 1) % n;
        new (m_buffer + m_back) T(element);
        ++m_size;
    }

    void push(T&& element)
    {
        if (m_size == n) {
            throw std::overflow_error("CircularQueue: size limit exceeded");
        }
        m_back = (m_back + 1) % n;
        new (m_buffer + m_back) T(std::move(element));
        ++m_size;
    }

    void pop()
    {
        if (m_size == 0) {
            throw std::underflow_error("CircularQueue: trying to pop from an empty queue");
        }
        m_buffer[m_front].~T();
        m_front = (m_front + 1) % n;
        --m_size;
    }

    T& operator[](std::size_t index)
    {
        if (index >= m_size) {
            throw std::out_of_range("CircularQueue: index out of bounds");
        }
        return m_buffer[(m_front + index) % n];
    }

    const T& operator[](std::size_t index) const
    {
        if (index >= m_size) {
            throw std::out_of_range("CircularQueue: index out of bounds");
        }
        return m_buffer[(m_front + index) % n];
    }

private:
    void clear() noexcept
    {
        while (m_size > 0) {
            pop();
        }
    }

    T* m_buffer;
    std::size_t m_front;
    std::size_t m_back;
    std::size_t m_size;
};

} // namespace frontend