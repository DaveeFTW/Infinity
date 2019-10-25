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

#include "flashingrectangle.h"
#include "vertextype.h"

#include <cmath>

#include <pspgu.h>
#include <pspgum.h>

FlashingRectangle::FlashingRectangle(const Rectangle& rect, unsigned int colour)
    : Rectangle(rect)
    , m_opacity(1.f)
    , m_colour(colour)
{
}

FlashingRectangle::FlashingRectangle(float x,
                                     float y,
                                     float width,
                                     float height,
                                     unsigned int colour)
    : Rectangle(x, y, width, height)
    , m_opacity(1.f)
    , m_colour(colour)
{
}

void FlashingRectangle::setColour(unsigned int colour)
{
    m_colour = colour;
}

void FlashingRectangle::setOpacity(float opacity)
{
    m_opacity = opacity;
}

void FlashingRectangle::update(float t)
{
    m_alpha = ((80.f / 2.f) * (sinf(2.5f * t) + 1) * m_opacity);
}

void FlashingRectangle::draw(void)
{
    auto vertices = static_cast<Vertex*>(sceGuGetMemory(2 * sizeof(Vertex)));
    auto adjustedColour = (m_colour & 0xFFFFFF) | (m_alpha << 24);

    vertices[0].colour = adjustedColour;
    vertices[0].x = x();
    vertices[0].y = y();
    vertices[0].z = 0.f;

    vertices[1].colour = adjustedColour;
    vertices[1].x = x() + width();
    vertices[1].y = y() + height();
    vertices[1].z = 0.f;

    sceGumDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2, 0, vertices);
}
