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

#include "font2.h"

namespace
{
    unsigned int blendAlpha(unsigned int colour, float alpha)
    {
        if (alpha > 1.0f)
            alpha = 1.f;
        if (alpha < 0.f)
            alpha = 0.f;

        return (colour & 0xFFFFFF) | ((unsigned int)(alpha * 255.f) << 24);
    }
}

Font::Font(const std::string& path)
    : m_font(nullptr)
    , m_size(0.8f)
    , m_colour(0xFFFFFFFF)
    , m_shadowColour(0)
    , m_angle(0.f)
    , m_opacity(1.f)
{
    m_font = intraFontLoad(path.c_str(), INTRAFONT_CACHE_ALL);
    intraFontSetEncoding(m_font, INTRAFONT_STRING_UTF8);
    intraFontSetStyle(m_font, m_size, blendAlpha(m_colour, m_opacity), m_shadowColour, m_angle, 0);
}

Font::Font(const unsigned char* data, size_t size)
    : m_font(nullptr)
    , m_size(0.8f)
    , m_colour(0xFFFFFFFF)
    , m_shadowColour(0)
    , m_angle(0.f)
    , m_opacity(1.f)
{
    m_font = intraFontLoadMemory(data, size, INTRAFONT_CACHE_ALL);
    intraFontSetEncoding(m_font, INTRAFONT_STRING_UTF8);
    intraFontSetStyle(m_font, m_size, blendAlpha(m_colour, m_opacity), m_shadowColour, m_angle, 0);
}

Font::~Font(void)
{
    if (m_font != nullptr)
        intraFontUnload(m_font);
}

void Font::setSize(float size)
{
    m_size = size;
    intraFontSetStyle(m_font, m_size, blendAlpha(m_colour, m_opacity), m_shadowColour, m_angle, 0);
}

float Font::size(void) const
{
    return m_size;
}

void Font::setColour(unsigned int colour)
{
    m_colour = colour;
    intraFontSetStyle(m_font, m_size, blendAlpha(m_colour, m_opacity), m_shadowColour, m_angle, 0);
}

unsigned int Font::colour(void) const
{
    return m_colour;
}

void Font::setShadowColour(unsigned int colour)
{
    m_shadowColour = colour;
    intraFontSetStyle(m_font, m_size, blendAlpha(m_colour, m_opacity), m_shadowColour, m_angle, 0);
}

unsigned int Font::shadowColour(void) const
{
    return m_shadowColour;
}

void Font::setAngle(float angle)
{
    m_angle = angle;
    intraFontSetStyle(m_font, m_size, blendAlpha(m_colour, m_opacity), m_shadowColour, m_angle, 0);
}

float Font::angle(void) const
{
    return m_angle;
}

void Font::setOpacity(float opacity)
{
    m_opacity = opacity;
    intraFontSetStyle(m_font, m_size, blendAlpha(m_colour, m_opacity), m_shadowColour, m_angle, 0);
}

float Font::opacity(void) const
{
    return m_opacity;
}

float Font::measureText(const std::string& text)
{
    return intraFontMeasureText(m_font, text.c_str());
}

float Font::calculateHeight(void)
{
    return m_font->advancey * m_font->size / 4.0f + 2.f;
}
