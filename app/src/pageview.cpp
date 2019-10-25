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

#include "pageview.h"
#include "animations.h"
#include "buttontransition.h"
#include "colouredrectangle.h"
#include "exitscreen.h"
#include "font2.h"
#include "fontmanager.h"
#include "functortransition.h"
#include "textrenderer.h"
#include "viewmanager.h"

#include "generated_file_data.h"

#include <cmath>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspkerneltypes.h>
#include <psploadexec.h>

PageView::PageView(ViewManager* mgr, const std::string& title, bool exitScreen)
    : m_viewManager(mgr)
    , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_symbols(new TextRenderer(
          new Font(files::symbol_font::data.data(), files::symbol_font::data.size())))
    , m_title(title)
    , m_titleBackground(new ColouredRectangle(0,
                                              0,
                                              480,
                                              m_latinText->font()->calculateHeight() + 4.f,
                                              0x44000000))
    , m_hasPageLeft(false)
    , m_hasPageRight(false)
    , m_visible(false)
    , m_t(0.f)
{
    m_symbols->font()->setColour(Font::WHITE);
    m_symbols->font()->setSize(1.2f);
    m_symbols->font()->setOpacity(0.75f);

    if (exitScreen)
    {
        m_exitScreen = std::make_unique<ExitScreen>(mgr);
        ButtonTransition* homeTransition = new ButtonTransition(PSP_CTRL_HOME);
        ButtonTransition* returnTransition = new ButtonTransition(PSP_CTRL_RTRIGGER);
        smoothFadeOutAnimation(this, m_exitScreen.get(), homeTransition);
        smoothFadeOutAnimation(m_exitScreen.get(), this, returnTransition);
        auto exitTransition = new FunctorTransition(
            [] {
                sceKernelExitGame();
                return true;
            },
            std::make_shared<ButtonTransition>(PSP_CTRL_CROSS));
        smoothFadeOutAnimation(m_exitScreen.get(), this, exitTransition);
    }
}

PageView::~PageView(void)
{
    delete m_latinText;
    delete m_symbols->font();
    delete m_symbols;
}

void PageView::setPageLeft(bool exists)
{
    m_hasPageLeft = exists;
}

void PageView::setPageRight(bool exists)
{
    m_hasPageRight = exists;
}

bool PageView::visible(void) const
{
    return m_visible;
}

void PageView::update(float t)
{
    m_t += t;

    double intpart;
    float fract = std::modf(m_t / 3.f, &intpart);
    auto parabola = fract * (1.f - fract);
    auto colourDiff = (int)(0xFF - 0xFF * (1.f / 4.f - parabola));

    m_symbols->font()->setColour(0xFF0000 | (colourDiff << 8) | colourDiff);
    m_symbols->font()->setOpacity(4.f * parabola);

    m_titleBackground->setOpacity(opacity());
    m_latinText->font()->setOpacity(opacity());
}

void PageView::render(void)
{
    sceGuDisable(GU_TEXTURE_2D);

    m_titleBackground->render();
    m_latinText->drawVerticalTop(5.f, 0.f, m_title);

    if (m_hasPageLeft)
        m_symbols->drawVerticalCenter(1.f, 272.f / 2.f, "c", TextRenderer::ALIGN_LEFT);

    if (m_hasPageRight)
        m_symbols->drawVerticalCenter(480.f - 1.f, 272.f / 2.f, "d", TextRenderer::ALIGN_RIGHT);
}

void PageView::begin(State* parent)
{
    m_viewManager->addView(this);
    m_visible = true;
}

void PageView::end(void)
{
    m_viewManager->removeView(this);
    m_visible = false;
}

ViewManager* PageView::viewManager(void)
{
    return m_viewManager;
}
