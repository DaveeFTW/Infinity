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

#include "installscreen.h"
#include "animations.h"
#include "application.h"
#include "buttonevent.h"
#include "colouredrectangle.h"
#include "font2.h"
#include "installupdatescreen.h"
#include "menuselectiontransition.h"
#include "tailedparticle.h"
#include "textrenderer.h"
#include "viewmanager.h"

#include <kernel_helper.h>
#include <libinfinity.h>

#include <cmath>
#include <iostream>
#include <sstream>

#include <pspctrl.h>
#include <pspgu.h>
#include <pspiofilemgr.h>

#include "vertextype.h"
#include <pspgum.h>

#include "numberanimation.h"

InstallScreen::InstallScreen(ViewManager* viewManager)
    : PageView(viewManager, "Install/Update Infinity")
    , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_opacity(1.0f)
    , m_t(0.f)
    , m_selectedItem(0)
    , m_menu(this)
    , m_particles(new TailedParticle(viewManager->graphics(), 175))
    , m_installScreen(new InstallUpdateScreen(viewManager, this))
{
    std::string install_message =
        (infGetVersion() < 0) ? "Press X to install Infinity" : "Press X to update Infinity";
    auto message_length = m_latinText->font()->measureText(install_message);

    m_menu.setX(480.f / 2.0f - (message_length + 25.0f) / 2.0f);
    m_menu.setY(272.f - 50.f);
    m_menu.setBackgroundSize(message_length + 25.0f, 30.f);

    auto installTransition = new MenuSelectionTransition(&m_menu, install_message);
    smoothFadeOutAnimation(this, m_installScreen.get(), installTransition);

    std::stringstream ver1;
    ver1 << "Infinity Version " << INFINITY_VERSION << std::endl;
    m_version = ver1.str();

    setOpacity(m_opacity);
}

void InstallScreen::setOpacity(float opacity)
{
    m_opacity = opacity;
    m_latinText->font()->setOpacity(m_opacity);
    m_particles->setOpacity(opacity);
}

float InstallScreen::opacity(void)
{
    return m_opacity;
}

void InstallScreen::update(float dt)
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

    m_menu.update(dt);
    PageView::update(dt);
}

void InstallScreen::render(void)
{
    sceGuDisable(GU_TEXTURE_2D);
    m_particles->draw();
    m_latinText->drawVerticalTop(480.f / 2.f, 50.f, m_version, TextRenderer::ALIGN_CENTER);
    m_latinText->font()->setColour(0xFF00FFFF);
    m_menu.draw();
    PageView::render();
}
