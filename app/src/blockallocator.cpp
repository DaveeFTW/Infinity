/*

Copyright (C) 2015, David "Davee" Morgan

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

#include "blockallocator.h"

BlockAllocator::BlockAllocator(void* base, unsigned int size)
    : m_base(base)
    , m_size(size)
{
}

void* BlockAllocator::allocate(unsigned int size)
{
    if (size > m_size)
    {
        return nullptr;
    }

    auto base = m_base;

    // align size to 16 so next pointer is aligned (if needed)
    if (size % 16)
    {
        size += 16 - (size % 16);
    }

    m_base = (char*)base + size;
    m_size -= size;
    return base;
}

void* BlockAllocator::allocate_aligned(unsigned int size, unsigned int alignment)
{
    // check if we need to do any alignment for the base pointer
    if ((unsigned int)m_base % alignment)
    {
        // just "allocate" the missing size
        allocate(alignment - ((unsigned int)m_base % alignment));
    }

    // now we should be correct alignment just allocate the memory as normal
    return allocate(size);
}
