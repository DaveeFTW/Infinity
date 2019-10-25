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

#include "infinityintro.h"
#include "blockallocator.h"
#include "circularbuffer.h"
#include "fontmanager.h"
#include "gltext.h"
#include "graphicsdevice.h"
#include "particlesource.h"
#include "pspguwrapper.h"
#include "viewmanager.h"

#include <intraFont.h>

#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>

#include <vector>

#include <limits.h>
#include <time.h>

#include <cmath>
#include <stdio.h>
#include <string.h>

// texture for particle
#include "texture.h"

char fps2[25];
typedef struct
{
    float decay;
    float decayRate;
    float x;
    float y;
    ScePspFVector2 direction;
} Sparkle;

class TailedParticle
{
public:
    TailedParticle(GraphicsDevice* device, int length)
        : m_graphicsDevice(device)
        , m_particleN(length)
        , m_particleSize(10.f)
        , m_vertices2(2 * m_particleN)
        , m_opacity(1.f)
        , m_particleSource(new ParticleSource(0, 0, 750))
    {
        m_particleSource->setRepeat(true);

        m_verticesVRAM = (TextureVertex*)m_graphicsDevice->vramAllocator()->allocate_aligned(
            2 * m_particleN * sizeof(TextureVertex), 16);

        // set default parameters for the vertex
        TextureVertex vertex;

        vertex.colour = 0;
        vertex.u = vertex.v = 0.0f;
        vertex.x = vertex.y = -1.f;
        vertex.z = 0.f;

        m_vertices.resize(m_particleN, vertex);

        for (int i = 0; i < m_particleN; ++i)
        {
            m_vertices2[(2 * i) + 1].u = m_vertices2[(2 * i) + 1].v = 127.f;
        }

        // load texture
        m_texture =
            (unsigned char*)m_graphicsDevice->vramAllocator()->allocate_aligned(128 * 128 * 4, 16);

        for (int i = 0; i < 128 * 128; ++i)
        {
            unsigned char color_cap = 0;

            if (g_infinityTex[i] > 0x90 && g_infinityTex[i] < 0xB0)
            {
                color_cap = 0x90;
            }

            m_texture[4 * i] = m_texture[4 * i + 1] = m_texture[4 * i + 2] =
                g_infinityTex[i] - color_cap;
            m_texture[4 * i + 3] = g_infinityTex[i];
        }
    }

    void moveTo(float x, float y)
    {
        TextureVertex vertex;

        vertex.colour = 0;
        vertex.u = vertex.v = 128.0f;
        vertex.x = x + m_particleSize / 2.0f;
        vertex.y = y + m_particleSize / 2.0f;
        vertex.z = 0.f;

        m_vertices2.push_front(vertex);

        vertex.u = vertex.v = 0.0f;
        vertex.x = x - m_particleSize / 2.0f;
        vertex.y = y - m_particleSize / 2.0f;

        m_vertices2.push_front(vertex);

        // update colours and size
        for (int i = 0; i < m_particleN; ++i)
        {
            unsigned char alpha =
                (215.f * (float)(m_particleN - i) / (float)m_particleN + 40.f) * m_opacity;

            m_vertices2[2 * i].colour = 0x00FFFFFF | ((int)(alpha * m_opacity) << 24);
            m_vertices2[(2 * i) + 1].colour = 0x00FFFFFF | ((int)(alpha * m_opacity) << 24);
        }

        m_particleSource->setSpawnArea(Rectangle(
            x - m_particleSize / 2.0f, y - m_particleSize / 2.0f, m_particleSize, m_particleSize));
    }

    void update(float dt) { m_particleSource->update(dt); }

    void draw(void)
    {
        // copy vertices
        copyCircularBuffer(m_verticesVRAM, m_vertices2);

        sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
        sceGuTexFunc(GU_TFX_ADD, GU_TCC_RGBA);
        sceGuTexImage(0, 128, 128, 128, m_texture);
        sceGuTexFilter(GU_NEAREST, GU_NEAREST);
        sceGuTexScale(1.0f / 128.f, 1.0f / 128.f);
        sceGuTexOffset(0.0f, 0.0f);
        sceGuAmbientColor(0xffffffff);

        sceKernelDcacheWritebackAll();
        sceGuTexFlush();
        sceGuTexSync();

        sceGuEnable(GU_TEXTURE_2D);
        sceGuDrawArray(GU_SPRITES,
                       GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
                       2 * m_particleN,
                       0,
                       m_verticesVRAM);
        sceGuDisable(GU_TEXTURE_2D);

        m_particleSource->render();
    }

    void setOpacity(float opacity) { m_opacity = opacity; }

private:
    template<class T>
    void copyCircularBuffer(void* dst, const CircularBuffer<T>& buffer)
    {
        memcpy(dst, buffer.array_one().first, buffer.array_one().second * sizeof(T));
        memcpy((char*)dst + buffer.array_one().second * sizeof(T),
               buffer.array_two().first,
               buffer.array_two().second * sizeof(T));
    }

private:
    GraphicsDevice* m_graphicsDevice;
    int m_particleN;
    float m_particleSize;
    std::vector<TextureVertex> m_vertices;
    TextureVertex* m_verticesVRAM;
    CircularBuffer<TextureVertex> m_vertices2;
    unsigned char* m_texture;
    float m_opacity;
    ParticleSource* m_particleSource;
};

InfinityScreen::InfinityScreen(ViewManager* manager)
    //: m_font(FontManager::instance())
    : m_t(0)
    , m_particles(new TailedParticle(manager->graphics(), 125))
    , m_text(new SimpleText(manager->graphics()->vramAllocator()))
    //, m_flashingRect(m_font->centeredRect(480.f/2.f, 3.f*(272.f/4.f), "Press X to start"),
    // 0x00000000)
    , m_viewManager(manager)
    , m_opacity(1.f)
{
    init();
}

void InfinityScreen::init(void)
{
    // set clear colour to black
    sceGuClearColor(0xff000000);
    sceGuClearDepth(0);

    // create vertices in vram
    m_vertices = (Vertex*)m_viewManager->graphics()->vramAllocator()->allocate_aligned(
        2 * sizeof(Vertex), 16);
}

void InfinityScreen::render(void)
{
    sceKernelDcacheWritebackAll();

    // m_flashingRect.draw();
    m_particles->draw();

    // m_font->drawCentered(480.f/2.f, 272.f/4.f, "Welcome to Infinity.");
    // m_text->drawCentered(480.f/2.f, 272.f/4.f, "Welcome to Infinity.");
    // m_font->drawCentered(480.f/2.f, 3.f*(272.f/4.f), "Press X to start");

    // m_text->draw(0, 0, "FPS: %s", fps2);
    // intraFontPrintf(m_font, 50, 50, "height: %f", m_font->advancey * m_font->size / 4.0f);
    // m_text->render();
}

void InfinityScreen::update(float dt)
{
    m_particles->setOpacity(m_opacity);
    // m_font->setOpacity(m_opacity);

    m_t += dt;

    if (dt > 0)
        sprintf(fps2, "%i", (int)(1.f / dt));

    float t = m_t;
    float scale = 2.0f / (3.0f - cosf(2.0f * t));

    float modelx = scale * cosf(t);
    float modely = scale * sinf(2.f * t) / 2.f;

    float width = 440.f;
    float height = 272.f;
    float xoffset = 480 - width - 5;
    float yoffset = 272 - height - 10;

    float x = width * ((modelx + 1) / 2.f) + xoffset / 2.f;
    float y = height * ((modely + 1) / 2.f) + yoffset;

    m_particles->moveTo(x, y);
    m_particles->update(dt);
    // m_flashingRect.update(m_t);
}

void InfinityScreen::setOpacity(float opacity)
{
    m_opacity = opacity;
    if (m_opacity > 1.f)
        m_opacity = 1.f;
    if (m_opacity < 0.f)
        m_opacity = 0.f;
}

float InfinityScreen::opacity(void)
{
    return m_opacity;
}

void InfinityScreen::begin(State* parent)
{
    m_viewManager->addView(this);
}

void InfinityScreen::end(void)
{
    m_viewManager->removeView(this);
}
