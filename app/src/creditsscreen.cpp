/*

Copyright (C) 2016, David "Davee" Morgan

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

#include "creditsscreen.h"
#include "backgroundview.h"
#include "firework.h"
#include "font2.h"
#include "numberanimation.h"
#include "textrenderer.h"
#include "viewmanager.h"

#include <algorithm>
#include <vector>

#include <time.h>

namespace
{
    float getRandomLinearPosition(auto ctx, float p, float length)
    {
        if (length == 0.f)
            return p;

        double rnd = sceKernelUtilsMt19937UInt(ctx) / (double)0xFFFFFFFF;
        return p + rnd * length;
    }

    std::vector<Firework> m_fireworks;
} // anonymous namespace

CreditsScreen::CreditsScreen(ViewManager* mgr, BackgroundView* bg)
    : m_viewManager(mgr)
    , m_background(bg)
    , m_visible(false)
    , m_t(0.f)
    , m_last_created(0.f)
    , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_currentCredits(0)
    , m_credits({
          "Special thanks to following people",
          "Nzaki - for whom this would not be possible",
          "The Z",
          "Darthsternie",
          "Marco Antony",
          "Balázs Triszka",
          "Hackinformer",
          "Wololo",
          "ME & PRO CFW developers",
          "Proxima",
          "libcg",
          "Modsiah",
          "Everyone who tested Infinity 2",
          "",
          "6.61 Infinity, by Davee",
          "Sleep tight, PSP",
          "Follow me on twitter: @DaveeFTW",
          "Read the technical write-up on https://lolhax.org",
      })
    , m_fadeOutBackground(new NumberAnimation(0, 0xFF - 0x15, 3 * 1000.f))
    , m_fadeInCredits(new NumberAnimation(0.f, 1.f, 300))
    , m_delayCredits(new NumberAnimation(0.f, 0.f, 5 * 1000.f))
    , m_fadeOutCredits(new NumberAnimation(0.f, 1.f, 300))
    , m_fadeOutComplete(false)
{
    auto easingOutCurve = [](float t, float b, float c, float d) {
        t /= d;
        return -c * t * (t - 2) + b;
    };

    m_fadeOutBackground->setEasing(easingOutCurve);
    m_fadeInCredits->setEasing(easingOutCurve);
    m_fadeOutCredits->setEasing(easingOutCurve);

    m_fadeOutBackground->setHandler([this](float modulation) {
        auto val = 0xFF - (unsigned char)modulation;
        auto color = 0xFF000000 | (val << 16) | (val << 8) | val;
        m_background->setModulationColour(color);
    });

    m_fadeOutBackground->onComplete([this]() {
        m_fadeInCredits->start();
        m_fadeOutComplete = true;
        return false;
    });

    m_fadeInCredits->setHandler(
        [this](float opacity) { m_latinText->font()->setOpacity(opacity); });

    m_fadeInCredits->onComplete([this]() {
        m_delayCredits->start();
        return false;
    });

    m_delayCredits->onComplete([this]() {
        m_fadeOutCredits->start();
        return false;
    });

    m_fadeOutCredits->setHandler(
        [this](float opacity) { m_latinText->font()->setOpacity(1 - opacity); });

    m_fadeOutCredits->onComplete([this]() {
        m_currentCredits++;

        if (m_currentCredits < m_credits.size())
            m_fadeInCredits->start();

        return false;
    });

    sceKernelUtilsMt19937Init(&m_ctx, time(NULL));
}

bool CreditsScreen::visible(void) const
{
    return m_visible;
}

void CreditsScreen::setOpacity(float opacity) {}

float CreditsScreen::opacity(void)
{
    return 1.f;
}

void CreditsScreen::update(float t)
{
    m_t += t;

    if (!m_fadeOutComplete)
        return;

    if (m_fireworks.size() < 20 && (m_t - m_last_created) > 0.75f)
    {
        auto rnd = sceKernelUtilsMt19937UInt(&m_ctx);
        auto x = getRandomLinearPosition(&m_ctx, 80, 400);
        auto y = getRandomLinearPosition(&m_ctx, 20, 180);
        Firework firework(x, y, rnd, 75);
        firework.setRepeat(true);
        firework.setDecayCoefficient(1.3f);
        firework.setVelocityCoefficient(75.f);

        m_fireworks.push_back(firework);
        m_last_created = m_t;
    }

    std::for_each(m_fireworks.begin(), m_fireworks.end(), [t, this](Firework& firework) {
        if (!firework.active())
        {
            auto x = getRandomLinearPosition(&m_ctx, 80, 400);
            auto y = getRandomLinearPosition(&m_ctx, 20, 180);

            firework.setSpawnArea(x, y);
        }

        firework.update(t);
    });
}

void CreditsScreen::render(void)
{
    if (!m_fadeOutComplete)
        return;

    std::for_each(
        m_fireworks.begin(), m_fireworks.end(), [](Firework& firework) { firework.render(); });

    if (m_currentCredits < m_credits.size())
    {
        m_latinText->draw(
            480.f / 2.f, 272.f / 2.f, m_credits[m_currentCredits], TextRenderer::ALIGN_CENTER);
    }
}

void CreditsScreen::begin(State* parent)
{
    m_fadeOutBackground->start();
    m_viewManager->addView(this);
    m_visible = true;
}

void CreditsScreen::end(void)
{
    m_viewManager->removeView(this);
    m_visible = false;
}

ViewManager* CreditsScreen::viewManager(void)
{
    return m_viewManager;
}
