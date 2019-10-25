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

#include "gltext.h"
#include "blockallocator.h"
#include "fontgen.h"

#include <pspgu.h>
#include <pspsdk.h>
#include <psputils.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

SimpleText::SimpleText(BlockAllocator* vram, int maxCharacters)
    : m_currentVertex(NULL)
{
    m_fontTexture = (char*)vram->allocate_aligned(
        128 * 128 * 4, 16); // getStaticVramTexture(128, 128, GU_PSM_8888);
    genFontTexture(m_fontTexture);

    // allocate vertex space too
    m_currentVertex = m_vertices = (TextureVertex*)vram->allocate_aligned(
        2 * maxCharacters * sizeof(TextureVertex),
        16); // getStaticVramTexture(2*maxCharacters, sizeof(TextureVertex), GU_PSM_T8);
}

void SimpleText::prepareVertices(float x, float y, const char* text)
{
    int len = strlen(text);

    for (int i = 0; i < len; ++i)
    {
        m_currentVertex->u = 8 * (text[i] % 16);
        m_currentVertex->v = 8 * (text[i] / 16);
        m_currentVertex->colour = 0xFFFFFFFF;
        m_currentVertex->x = x + (i * 8);
        m_currentVertex->y = y;
        m_currentVertex->z = 0;

        m_currentVertex++;

        m_currentVertex->u = 8 * (text[i] % 16) + 8;
        m_currentVertex->v = 8 * (text[i] / 16) + 8;
        m_currentVertex->colour = 0xFFFFFFFF;
        m_currentVertex->x = x + (i * 8) + 8;
        m_currentVertex->y = y + 8;
        m_currentVertex->z = 0;

        m_currentVertex++;
    }
}

void SimpleText::draw(float x, float y, const char* fmt, ...)
{
    char msg[256];
    va_list list;

    /* collate args into string */
    va_start(list, fmt);
    vsprintf(msg, fmt, list);
    va_end(list);

    prepareVertices(x, y, msg);
}

void SimpleText::draw(float x, float y, const std::string& fmt, ...)
{
    char msg[256];
    va_list list;

    /* collate args into string */
    va_start(list, fmt.c_str());
    vsprintf(msg, fmt.c_str(), list);
    va_end(list);

    prepareVertices(x, y, msg);
}

void SimpleText::drawCentered(float x, float y, const char* fmt, ...)
{
    char msg[256];
    va_list list;

    /* collate args into string */
    va_start(list, fmt);
    vsprintf(msg, fmt, list);
    va_end(list);

    int len = strlen(msg);

    // each character is 8 bits wide and we need half of it
    prepareVertices(x - len * 4, y - 4, msg);
}

void SimpleText::drawCentered(float x, float y, const std::string& fmt, ...)
{
    char msg[256];
    va_list list;

    /* collate args into string */
    va_start(list, fmt.c_str());
    vsprintf(msg, fmt.c_str(), list);
    va_end(list);

    // each character is 8 bits wide and we need half of it
    prepareVertices(x - fmt.length() * 4, y - 4, msg);
}

void SimpleText::render(void)
{
    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexImage(0, 128, 128, 128, m_fontTexture);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexScale(1.0f / 128.f, 1.0f / 128.f);
    sceGuAmbientColor(0xffffffff);

    sceKernelDcacheWritebackAll();

    sceGuEnable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_SPRITES,
                   GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
                   ((int)m_currentVertex - (int)m_vertices) / sizeof(TextureVertex),
                   0,
                   m_vertices);
    sceGuDisable(GU_TEXTURE_2D);

    // reset current vertex
    m_currentVertex = m_vertices;
}
