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

#ifndef FONT_H
#define FONT_H

#include <intraFont.h>
#include <string>

class Font
{
    friend class TextRenderer;

public:
    enum Colour
    {
        WHITE = 0xFFFFFFFF,
        BLACK = 0x00000000,
        RED = 0xFF0000FF,
        BLUE = 0xFFFF0000,
        GREEN = 0xFF00FF00
    };

public:
    explicit Font(const std::string& path);
    Font(const unsigned char* data, size_t size);
    ~Font(void);

    void setSize(float size);
    float size(void) const;
    void setColour(unsigned int colour);
    unsigned int colour(void) const;
    void setShadowColour(unsigned int colour);
    unsigned int shadowColour(void) const;
    void setAngle(float angle);
    float angle(void) const;
    void setOpacity(float opacity);
    float opacity(void) const;

    float measureText(const std::string& text);
    float calculateHeight(void);

private:
    intraFont* m_font;
    float m_size;
    unsigned int m_colour;
    unsigned int m_shadowColour;
    float m_angle;
    float m_opacity;
};

#endif // FONT_H
