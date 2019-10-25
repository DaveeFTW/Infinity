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

#include "menu.h"
#include "application.h"
#include "buttonevent.h"
#include "colouredrectangle.h"
#include "font2.h"
#include "menuselectionevent.h"
#include "pageview.h"
#include "textrenderer.h"

#include <cmath>

#include <pspctrl.h>

Menu::Menu(PageView* parent, std::initializer_list<std::string> l)
    : m_parent(parent)
    , m_list(l)
    , m_selectedItem(0)
    , m_selectionColour(Font::WHITE)
    , m_selection(false)
    , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_t(0.f)
    , m_x(0.f)
    , m_y(0.f)
{
    Application::instance()->addEventHandler([=](Event* event) {
        if (m_parent->visible())
        {
            onEvent(event);
        }
    });
}

void Menu::setX(float x)
{
    m_x = x;
}

float Menu::x(void)
{
    return m_x;
}

void Menu::setY(float y)
{
    m_y = y;
}

float Menu::y(void)
{
    return m_y;
}

void Menu::addItem(const std::string& item)
{
    m_list.push_back(item);
}

void Menu::clear(void)
{
    m_list.clear();
}

void Menu::update(float dt)
{
    m_t += dt / 2.f;

    auto intpart = 0.0;
    float x = std::modf(m_t, &intpart);

    auto easingInOut = [](float t, float b, float c, float d) {
        t /= d / 2;
        if (t < 1)
            return c / 2 * t * t + b;
        t--;
        return -c / 2 * (t * (t - 2) - 1) + b;
    };

    auto y = easingInOut(x, 0.f, 2.f, 1.f);
    unsigned char colour = std::floor(255.f * ((y > 1.f) ? (2.f - y) : (y)));
    m_selectionColour = 0x9900FFFF | (colour << 16);
}

void Menu::setBackgroundSize(float width, float height)
{
    m_bgWidth = width;
    m_bgHeight = height;
}

void Menu::draw(void)
{
    m_latinText->font()->setOpacity(m_parent->opacity());

    if (m_drawbg)
    {
        ColouredRectangle body(x(), y(), m_bgWidth, m_bgHeight, 0x33000000);
        body.setOpacity(m_parent->opacity());
        body.render();
    }

    for (auto i = (unsigned)0; i < m_list.size(); ++i)
    {
        m_latinText->font()->setColour((unsigned)m_selectedItem == i ? (m_selectionColour)
                                                                     : (Font::WHITE));
        m_latinText->drawVerticalTop(
            480.f / 2.f, y() + 5.f + 15.f * i, m_list[i], TextRenderer::ALIGN_CENTER);
    }
}

void Menu::drawBackground(bool drawbg)
{
    m_drawbg = drawbg;
}

EventList Menu::processEvents(void)
{
    EventList list;

    if (m_selection)
    {
        list.push_back(new MenuSelectionEvent(this, m_list[m_selectedItem], m_selectedItem));
        m_selection = false;
    }

    return list;
}

void Menu::onEvent(Event* event)
{
    if (m_list.size() == 0)
        return;

    if (event->type() != Event::Button)
        return;

    ButtonEvent* buttonEvent = static_cast<ButtonEvent*>(event);

    switch (buttonEvent->buttons())
    {
        case PSP_CTRL_UP:
        {
            m_selectedItem--;

            if (m_selectedItem < 0)
                m_selectedItem = m_list.size() - 1;

            break;
        }

        case PSP_CTRL_DOWN:
        {
            m_selectedItem++;

            if ((unsigned)m_selectedItem >= m_list.size())
                m_selectedItem = 0;

            break;
        }

        case PSP_CTRL_CROSS:
        {
            m_selection = true;
            break;
        }

        default:
            // do nothing
            break;
    }
}
