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

#include "backgroundview.h"
#include "particlesource.h"
#include "texture2.h"
#include "vertextype.h"

#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>

#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

BackgroundView::BackgroundView(ViewManager* manager)
    : m_viewManager(manager)
    , m_particles(new ParticleSource(Rectangle(0, 0, 480, 272), 150.f))
    , m_textureLeft(nullptr)
    , m_textureRight(nullptr)
    , m_modulationColour(0xFFFFFFFF)
{
    m_particles->setDecayCoefficient(3.f);
    m_particles->setVelocityCoefficient(1.f);
    m_particles->setRepeat(true);
    m_particles->setOpacity(opacity());

    gumLoadIdentity(&m_savedModel);
}

BackgroundView::~BackgroundView(void) {}

void BackgroundView::setTextureLeft(Texture* texture)
{
    m_textureLeft = texture;
    m_textureLeft->swizzle();
}

void BackgroundView::setTextureRight(Texture* texture)
{
    m_textureRight = texture;
    m_textureRight->swizzle();
}

void BackgroundView::setModulationColour(unsigned int colour)
{
    m_modulationColour = colour;
}

const ScePspFMatrix4* BackgroundView::loadModel(void) const
{
    return &m_savedModel;
}

void BackgroundView::saveModel(const ScePspFMatrix4* model)
{
    gumLoadMatrix(&m_savedModel, model);
}

void BackgroundView::render(void)
{
    if (m_textureLeft == nullptr || m_textureRight == nullptr)
    {
        return;
    }

    auto vertices = static_cast<TextureVertex*>(sceGuGetMemory(sizeof(TextureVertex) * 4));

    vertices[0].colour = m_modulationColour;
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[0].x = -((m_textureLeft->width() + m_textureRight->width()) - SCR_WIDTH) / 2.f;
    vertices[0].y = 0.0f;
    vertices[0].z = 0.2f;

    vertices[1].colour = m_modulationColour;
    vertices[1].u = m_textureLeft->width();
    vertices[1].v = m_textureLeft->height();
    vertices[1].x = SCR_WIDTH / 2.f;
    vertices[1].y = SCR_HEIGHT;
    vertices[1].z = 0.2f;

    vertices[2].colour = m_modulationColour;
    vertices[2].u = 0;
    vertices[2].v = 0;
    vertices[2].x = SCR_WIDTH / 2.f;
    vertices[2].y = 0;
    vertices[2].z = 0.2f;

    vertices[3].colour = m_modulationColour;
    vertices[3].u = m_textureRight->width();
    vertices[3].v = m_textureRight->height();
    vertices[3].x =
        SCR_WIDTH + ((m_textureLeft->width() + m_textureRight->width()) - SCR_WIDTH) / 2.f;
    vertices[3].y = SCR_HEIGHT;
    vertices[3].z = 0.2f;

    sceGuTexMode(GU_PSM_8888, 0, 0, GU_TRUE);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGB);
    sceGuTexImage(0,
                  m_textureLeft->textureWidth(),
                  m_textureLeft->textureHeight(),
                  m_textureLeft->textureWidth(),
                  m_textureLeft->data());

    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexScale(1.f / m_textureLeft->textureWidth(), 1.f / m_textureLeft->textureHeight());
    sceGuAmbientColor(0xffffffff);

    sceKernelDcacheWritebackAll();

    sceGuEnable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_SPRITES,
                    GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                    2,
                    0,
                    vertices);

    sceGuTexMode(GU_PSM_8888, 0, 0, GU_TRUE);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGB);
    sceGuTexImage(0,
                  m_textureRight->textureWidth(),
                  m_textureRight->textureHeight(),
                  m_textureRight->textureWidth(),
                  m_textureRight->data());

    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexScale(1.f / m_textureRight->textureWidth(), 1.f / m_textureRight->textureHeight());
    sceGuAmbientColor(0xffffffff);
    sceKernelDcacheWritebackAll();
    sceGumDrawArray(GU_SPRITES,
                    GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                    2,
                    0,
                    vertices + 2);

    sceGuDisable(GU_TEXTURE_2D);
    m_particles->render();
}

void BackgroundView::update(float dt)
{
    m_particles->update(dt);
}
