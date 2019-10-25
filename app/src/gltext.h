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

#ifndef GLTEXT_H
#define GLTEXT_H

#include "vertextype.h"

#include <string>

class BlockAllocator;

class SimpleText
{
public:
    SimpleText(BlockAllocator* vram, int maxCharacters = 500);

    void draw(float x, float y, const char* text, ...);
    void draw(float x, float y, const std::string& fmt, ...);

    void drawCentered(float x, float y, const char* text, ...);
    void drawCentered(float x, float y, const std::string& text, ...);

    void render(void);

private:
    void prepareVertices(float x, float y, const char* text);

private:
    TextureVertex* m_currentVertex;
    char* m_fontTexture;
    TextureVertex* m_vertices;
};

#endif // GLTEXT_H
