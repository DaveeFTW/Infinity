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

#include "fontgen.h"
#include "glfont.h"

static int isBitSet(unsigned char container, int n)
{
    return (container & (1 << n)) ? (1) : (0);
}

int genFontTexture(char* dst)
{
    unsigned int* rgba8888 = (unsigned int*)dst;

    for (unsigned int i = 0; i < sizeof(g_generated_font); ++i)
    {
        unsigned char ch = g_generated_font[i];

        for (unsigned int j = 0; j < 8; ++j)
        {
            *rgba8888 = 0x00FFFFFF | (isBitSet(ch, j) ? (0xFF000000) : (0));
            rgba8888++;
        }
    }

    // return the size
    return rgba8888 - (unsigned int*)dst;
}
