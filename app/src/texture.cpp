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

#include "texture2.h"

#include <cstring>
#include <psptypes.h>

Texture::Texture(void)
    : m_swizzled(false)
{
}

Texture::~Texture(void) {}

bool Texture::swizzled(void) const
{
    return m_swizzled;
}

void Texture::swizzle(void)
{
    unsigned int rowblocks = (textureWidth() * sizeof(u32) / 16);
    auto size = textureWidth() * textureHeight() * pixelSize();

    auto out = new char[size];

    for (auto j = 0; j < textureHeight(); ++j)
    {
        for (auto i = 0; i < textureWidth() * (int)sizeof(u32); ++i)
        {
            unsigned int blockx = i / 16;
            unsigned int blocky = j / 8;

            unsigned int x = (i - blockx * 16);
            unsigned int y = (j - blocky * 8);
            unsigned int block_index = blockx + ((blocky)*rowblocks);
            unsigned int block_address = block_index * 16 * 8;

            out[block_address + x + y * 16] = data()[i + j * textureWidth() * sizeof(u32)];
        }
    }

    std::memcpy(data(), out, size);
    m_swizzled = !m_swizzled;
    delete[] out;
}
