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

#include <stddef.h>

unsigned int strlen(const char* s)
{
    int len = 0;
    while (*s)
    {
        s++;
        len++;
    }

    return len;
}

void* memcpy(void* dst, const void* src, size_t len)
{
    void* pRet = dst;
    const char* usrc = (const char*)src;
    char* udst = (char*)dst;

    while (len > 0)
    {
        *udst++ = *usrc++;
        len--;
    }

    return pRet;
}

int memcmp(const void* b1, const void* b2, size_t len)
{
    int val = 0;
    const unsigned char *u1, *u2;

    u1 = (const unsigned char*)b1;
    u2 = (const unsigned char*)b2;

    while (len > 0)
    {
        if (*u1 != *u2)
        {
            val = (int)*u1 - (int)*u2;
            break;
        }
        u1++;
        u2++;
        len--;
    }

    return val;
}

void* memmove(void* dst, const void* src, size_t len)
{
    void* pRet = dst;
    char* udst;
    const char* usrc;

    if (dst < src)
    {
        /* Copy forwards */
        udst = (char*)dst;
        usrc = (const char*)src;
        while (len > 0)
        {
            *udst++ = *usrc++;
            len--;
        }
    }
    else
    {
        /* Copy backwards */
        udst = ((char*)dst) + len;
        usrc = ((const char*)src) + len;
        while (len > 0)
        {
            *--udst = *--usrc;
            len--;
        }
    }

    return pRet;
}

void* memset(void* b, int c, size_t len)
{
    void* pRet = b;
    unsigned char* ub = (unsigned char*)b;

    while (len > 0)
    {
        *ub++ = (unsigned char)c;
        len--;
    }

    return pRet;
}
