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

#include "colouredrectangle.h"
#include "vertextype.h"

#include <pspgu.h>
#include <pspgum.h>

ColouredRectangle::ColouredRectangle(float x,
                                     float y,
                                     float width,
                                     float height,
                                     unsigned int colour)
    : Rectangle(x, y, width, height)
    , m_colour(colour)
    , m_opacity(1.f)
{
}

ColouredRectangle::ColouredRectangle(const Rectangle& rectangle, unsigned int colour)
    : Rectangle(rectangle)
    , m_colour(colour)
    , m_opacity(1.f)
{
}

void ColouredRectangle::setOpacity(float opacity)
{
    m_opacity = opacity;
}

float ColouredRectangle::opacity(void)
{
    return m_opacity;
}

void ColouredRectangle::render(void)
{
    auto vertices = static_cast<Vertex*>(sceGuGetMemory(2 * sizeof(Vertex)));
    auto adjustedColour = ((int)((m_colour >> 24) * m_opacity) << 24) | (m_colour & 0xFFFFFF);

    vertices[0].x = x();
    vertices[0].y = y();
    vertices[0].z = 0.f;
    vertices[0].colour = adjustedColour;

    vertices[1].x = x() + width();
    vertices[1].y = y() + height();
    vertices[1].z = 0.f;
    vertices[1].colour = adjustedColour;

    sceGuDisable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2, 0, vertices);
}
