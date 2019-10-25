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

#include "homescreen.h"
#include "blockallocator.h"
#include "font2.h"
#include "fontmanager.h"
#include "graphicsdevice.h"
#include "particlesource.h"
#include "tailedparticle.h"
#include "textrenderer.h"
#include "vertextype.h"
#include "viewmanager.h"

#include <pspgu.h>
#include <pspgum.h>

#include <math.h>

HomeScreen::HomeScreen(ViewManager* viewManager)
    : PageView(viewManager, "Home")
    , m_latinText(new TextRenderer(FontManager::defaultFont()))
    , m_particles(new TailedParticle(viewManager->graphics(), 175))
    , m_opacity(1.0f)
    , m_t(0.0f)
{
}

void HomeScreen::setOpacity(float opacity)
{
    m_opacity = opacity;
    m_latinText->font()->setOpacity(m_opacity);
    m_particles->setOpacity(opacity);
}

float HomeScreen::opacity(void)
{
    return m_opacity;
}

void HomeScreen::update(float dt)
{
    m_t += dt;

    float t = m_t / 1.75f;
    float scale = 2.0f / (3.0f - cosf(2.0f * t));

    float modelx = scale * cosf(t);
    float modely = scale * sinf(2.f * t) / 2.f;

    float width = 400.f;
    float height = 400.f;
    float xoffset = 480 - width - 5;
    float yoffset = 272.f / 2.f - height / 2.f;

    float x = width * ((modelx + 1) / 2.f) + xoffset / 2.f;
    float y = height * ((modely + 1) / 2.f) + yoffset;

    m_particles->moveTo(x, y);
    m_particles->update(dt);

    PageView::update(dt);
}

void HomeScreen::render(void)
{
    sceGuDisable(GU_TEXTURE_2D);
    m_latinText->drawVerticalTop(
        480.f / 2.f, 50.f, "Welcome to Infinity.", TextRenderer::ALIGN_CENTER);
    m_particles->draw();
    PageView::render();
}
