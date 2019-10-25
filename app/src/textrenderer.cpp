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

#include "textrenderer.h"
#include "font2.h"
#include "fontmanager.h"

#include <pspgum.h>

TextRenderer::TextRenderer(void) {}

TextRenderer::TextRenderer(Font* font)
    : m_font(font)
{
}

#include <stdio.h>

int TextRenderer::draw(float x, float y, const std::string& text, Alignment align)
{
    switch (align)
    {
        default:
        case ALIGN_LEFT:
            m_font->m_font->options =
                (m_font->m_font->options & ~PGF_ALIGN_MASK) | INTRAFONT_ALIGN_LEFT;
            break;
        case ALIGN_CENTER:
            m_font->m_font->options =
                (m_font->m_font->options & ~PGF_ALIGN_MASK) | INTRAFONT_ALIGN_CENTER;
            break;
        case ALIGN_RIGHT:
            m_font->m_font->options =
                (m_font->m_font->options & ~PGF_ALIGN_MASK) | INTRAFONT_ALIGN_RIGHT;
            break;
    }

    ScePspFMatrix4 model;
    sceGumStoreMatrix(&model);

    float transformedX = model.x.x * x + model.y.x * y + model.z.x * 0.f + model.w.x * 1.f;
    float transformedY = model.x.y * x + model.y.y * y + model.z.y * 0.f + model.w.y * 1.f;

    return intraFontPrint(m_font->m_font, transformedX, transformedY, text.c_str());
}

int TextRenderer::drawVerticalTop(float x, float y, const std::string& text, Alignment align)
{
    float height = m_font->calculateHeight();
    return draw(x, y + height, text, align);
}

int TextRenderer::drawVerticalCenter(float x, float y, const std::string& text, Alignment align)
{
    float height = m_font->calculateHeight();
    return draw(x, y + height / 2.f - 2, text, align);
}

Rectangle TextRenderer::rectangle(float x, float y, const std::string& text)
{
    float width = m_font->measureText(text);
    float height = m_font->calculateHeight();
    return Rectangle(x - width / 2.f, y - height / 2.f, width, height);
}

void TextRenderer::setFont(Font* font)
{
    m_font = font;
}

Font* TextRenderer::font(void) const
{
    return m_font;
}
