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

#include "tgatexture.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

namespace
{
    int nextPow2(int value)
    {
        auto i = 1;
        while (value > i)
            i <<= 1;
        return i;
    }
}

TgaTexture::TgaTexture(const unsigned char* data, size_t size)
    : m_data(nullptr)
    , m_imageWidth(0)
    , m_imageHeight(0)
    , m_textureWidth(0)
    , m_textureHeight(0)
    , m_pixelSize(0)
{
    load(data, size);
}

TgaTexture::~TgaTexture(void)
{
    if (m_data)
        free(m_data);
}

int TgaTexture::width(void) const
{
    return m_imageWidth;
}

int TgaTexture::height(void) const
{
    return m_imageHeight;
}

int TgaTexture::textureWidth(void) const
{
    return m_textureWidth;
}

int TgaTexture::textureHeight(void) const
{
    return m_textureHeight;
}

int TgaTexture::pixelSize(void) const
{
    return m_pixelSize;
}

unsigned char* TgaTexture::data(void)
{
    return m_data;
}

bool TgaTexture::load(const unsigned char* data, size_t size)
{
    // Variables to hold image info
    unsigned char unCompressHeader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    const unsigned char* header = data + 12;

    // We only want to read uncompressed TGA's. Chech the
    // header if it is of an uncompressed one.
    if (unCompressHeader[2] != 2)
    {
        printf("error, tga not uncompressed\n");
        return false;
    }

    // Calculate and save the Width & Height of Image
    m_imageWidth = header[1] * 256 + header[0];
    m_imageHeight = header[3] * 256 + header[2];
    auto bitCount = header[4];

    // calculate the texture width
    m_textureWidth = nextPow2(m_imageWidth);
    m_textureHeight = nextPow2(m_imageHeight);

    // only support 32 bit TGA
    if (bitCount != 32)
    {
        printf("require 32 bit tga, got %i\n", bitCount);
        return false;
    }

    m_pixelSize = bitCount / 8;

    // Allocate memory for the image and load it
    m_data =
        static_cast<unsigned char*>(memalign(16, m_textureWidth * m_textureHeight * m_pixelSize));

    // read in TGA line-by-line
    for (auto y = 0, pos = 0; y < m_imageHeight; y++, pos += m_textureWidth * m_pixelSize)
    {
        memcpy(
            m_data + pos, data + 18 + y * (m_imageWidth * m_pixelSize), m_imageWidth * m_pixelSize);

        // convert to PSP RGB format
        for (auto x = 0; x < m_imageWidth * m_pixelSize; x += m_pixelSize)
        {
            std::swap(m_data[pos + x], m_data[pos + x + 2]);
        }
    }

    return true;
}
