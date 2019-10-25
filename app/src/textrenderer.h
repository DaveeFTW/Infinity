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

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "rectangle.h"
#include <string>

class Font;

class TextRenderer
{
public:
    enum Alignment
    {
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT
    };

public:
    TextRenderer(void);
    TextRenderer(Font* font);

    int draw(float x, float y, const std::string& text, Alignment align = ALIGN_LEFT);
    int drawVerticalCenter(float x, float y, const std::string& text, Alignment align = ALIGN_LEFT);
    int drawVerticalTop(float x, float y, const std::string& text, Alignment align = ALIGN_LEFT);
    Rectangle rectangle(float x, float y, const std::string& text);

    void setFont(Font* font);
    Font* font(void) const;

private:
    Font* m_font;
};

#endif // TEXTRENDERER_H
