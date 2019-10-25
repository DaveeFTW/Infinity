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

#include "rectangle.h"

Rectangle::Rectangle(void)
    : m_x(0.f)
    , m_y(0.f)
    , m_width(0.f)
    , m_height(0.f)
{
}

Rectangle::Rectangle(float x, float y, float width, float height)
    : m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
{
}

void Rectangle::setX(float x)
{
    m_x = x;
}

float Rectangle::x(void) const
{
    return m_x;
}

void Rectangle::setY(float y)
{
    m_y = y;
}

float Rectangle::y(void) const
{
    return m_y;
}

void Rectangle::setWidth(float width)
{
    m_width = width;
}

float Rectangle::width(void) const
{
    return m_width;
}

void Rectangle::setHeight(float height)
{
    m_height = height;
}

float Rectangle::height(void) const
{
    return m_height;
}
