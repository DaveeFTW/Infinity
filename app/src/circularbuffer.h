/*

Copyright (C) 2019, David "Davee" Morgan

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.


 */

#pragma once

#include <algorithm>
#include <vector>

template<typename T>
class CircularBuffer
{
public:
    CircularBuffer(std::size_t capacity)
    {
        m_data.resize(capacity);
        m_head = m_tail = m_data.begin();
    }

    void push_front(const T& value)
    {
        auto prev = std::prev(m_head);

        if (prev < m_data.begin())
        {
            prev = std::prev(m_data.end());
        }

        if (std::distance(prev, m_tail) == 0)
        {
            auto tail_prev = std::prev(m_tail);

            if (tail_prev < m_data.begin())
            {
                tail_prev = std::prev(m_data.end());
            }

            m_tail = tail_prev;
        }

        m_head = prev;
        *m_head = value;
    }

    bool is_linear() const { return (m_head < m_tail); }

    std::pair<const T*, std::size_t> array_one() const
    {
        typename std::vector<T>::const_iterator const_head = m_head;

        if (is_linear())
        {
            return { &(*m_head), std::distance(m_head, m_tail) };
        }

        return { &(*m_head), std::distance(const_head, m_data.end()) };
    }

    std::pair<const T*, std::size_t> array_two() const
    {
        typename std::vector<T>::const_iterator const_tail = m_tail;

        if (is_linear())
        {
            return { nullptr, 0 };
        }

        return { m_data.data(), std::distance(m_data.begin(), const_tail) };
    }

    T& operator[](std::size_t pos)
    {
        if (is_linear())
        {
            return *std::next(m_head, pos);
        }

        auto head_distance = std::distance(m_head, m_data.end());
        if (pos < static_cast<std::size_t>(head_distance))
        {
            return *std::next(m_head, pos);
        }
        else
        {
            return *std::next(m_data.begin(), pos - head_distance);
        }
    }

private:
    std::vector<T> m_data;
    typename std::vector<T>::iterator m_head, m_tail;
};