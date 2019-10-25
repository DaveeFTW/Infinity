/*

Copyright (C) 2019, David "Davee" Morgan

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

#include "exitscreen.h"
#include "animations.h"
#include "application.h"
#include "buttonevent.h"
#include "colouredrectangle.h"
#include "flashingrectangle.h"
#include "font2.h"
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

ExitScreen::ExitScreen(ViewManager* manager)
    : m_viewManager(manager)
    , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_rectangle(std::make_unique<ColouredRectangle>(0, 0, 480, 272, 0xFF000000))
    , m_opacity(1.0f)
    , m_t(0.f)
{
}

void ExitScreen::setOpacity(float opacity)
{
    m_opacity = opacity;
    m_latinText->font()->setOpacity(m_opacity);
    m_rectangle->setOpacity(m_opacity);
}

float ExitScreen::opacity(void)
{
    return m_opacity;
}

void ExitScreen::update(float dt) {}

void ExitScreen::render(void)
{
    sceGuDisable(GU_TEXTURE_2D);
    m_rectangle->render();
    m_latinText->draw(480.f / 2.f,
                      272.f / 2.f,
                      "Press X to Exit, or R Trigger to return.",
                      TextRenderer::ALIGN_CENTER);
}

void ExitScreen::begin(State* parent)
{
    m_viewManager->addView(this);
}

void ExitScreen::end(void)
{
    m_viewManager->removeView(this);
}
