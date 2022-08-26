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

#include "configscreen.h"
#include "application.h"
#include "colouredrectangle.h"
#include "compatibilitymodule.h"
#include "font2.h"
#include "menuselectionevent.h"
#include "textrenderer.h"

#include <config.h>

#include <pspctrl.h>

#include <algorithm>
#include <iostream>
#include <sstream>

ConfigScreen::ConfigScreen(ViewManager* viewManager)
    : PageView(viewManager, "Configuration")
    , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_opacity(1.0f)
    , m_selectedItem(0)
    , m_menu(this)
{
    m_menu.setX(36.f);
    m_menu.setY(75.f);

    Application::instance()->addEventHandler([=](Event* event) {
        if (visible())
        {
            onEvent(event);
        }
    });

    int res = infInitConfig(INFINITY_CONFIGURATION_LOCATION);

    if (res < 0)
    {
        res = infCreateConfig(INFINITY_CONFIGURATION_LOCATION);

        if (res < 0)
        {
            std::stringstream err;
            err << "Could not initialise libconfig (" << std::hex << res << ")";
            m_libconfigStatus = err.str();
        }
    }

    if (res >= 0)
    {
        auto size = 0;
        res = infGetEntryInfo("inf_compat_installed", &size);

        if (res >= 0)
        {
            auto buffer = new char[size];

            res = infGetEntry("inf_compat_installed", buffer, size);

            if (res >= 0)
            {
                std::string json(buffer, size);
                m_list = parseCompatibilityConfig(json);
            }

            delete[] buffer;
        }

        m_libconfigStatus = "Installed modules";

        std::string basejson =
            "[{\"name\":\"PRO "
            "CFW\",\"author\":\"Coldbird\",\"path\":\"flash0:/kd/procompat.bin\"},{\"name\":\"ME "
            "CFW\",\"author\":\"neur0n\",\"path\":\"flash0:/kd/mecompat.bin\"}, "
            "{\"name\":\"ARK CFW\",\"author\":\"Acid_Snake\",\"path\":\"flash0:/kd/arkcompat.bin\"}]";
        auto list = parseCompatibilityConfig(basejson);
        m_list.insert(m_list.begin(), list.begin(), list.end());

        char compat_path[256];
        auto isEnabledModule =
            infGetEntry("inf_boot_compat_path", compat_path, sizeof(compat_path)) >= 0;
        std::string enabledModule(compat_path);
        m_menu.clear();

        for (auto i = 0u; i < m_list.size(); ++i)
        {
            if (isEnabledModule && enabledModule == m_list[i]->path())
            {
                m_menu.addItem("* " + m_list[i]->name() + " by " + m_list[i]->author());
            }
            else
            {
                m_menu.addItem(m_list[i]->name() + " by " + m_list[i]->author());
            }
        }
    }
}

void ConfigScreen::setOpacity(float opacity)
{
    m_opacity = opacity;
    m_latinText->font()->setOpacity(m_opacity);
}

float ConfigScreen::opacity(void)
{
    return m_opacity;
}

void ConfigScreen::update(float dt)
{
    m_menu.update(dt);
    PageView::update(dt);
}

void ConfigScreen::render(void)
{
    m_latinText->drawVerticalTop(480.f / 2.f, 50.f, m_libconfigStatus, TextRenderer::ALIGN_CENTER);
    m_menu.draw();
    PageView::render();
}

void ConfigScreen::onEvent(Event* event)
{
    if (m_list.size() == 0)
        return;

    if (event->type() != Event::MenuSelection)
        return;

    MenuSelectionEvent* menuSelection = static_cast<MenuSelectionEvent*>(event);

    char compat_path[256];
    auto isEnabledModule =
        infGetEntry("inf_boot_compat_path", compat_path, sizeof(compat_path)) >= 0;
    std::string enabledModule(compat_path);

    auto index = menuSelection->index();

    if (isEnabledModule && enabledModule == m_list[index]->path())
    {
        // already enabled, so disable
        infUpdateEntry("inf_boot_compat_path", "", 1);
    }

    else if (infAddEntry("inf_boot_compat_path",
                         m_list[index]->path().c_str(),
                         m_list[index]->path().length() + 1) < 0)
    {
        infUpdateEntry("inf_boot_compat_path",
                       m_list[index]->path().c_str(),
                       m_list[index]->path().length() + 1);
    }

    isEnabledModule = infGetEntry("inf_boot_compat_path", compat_path, sizeof(compat_path)) >= 0;
    enabledModule = std::string(compat_path);
    m_menu.clear();

    for (auto i = 0u; i < m_list.size(); ++i)
    {
        if (isEnabledModule && enabledModule == m_list[i]->path())
        {
            m_menu.addItem("* " + m_list[i]->name() + " by " + m_list[i]->author());
        }
        else
        {
            m_menu.addItem(m_list[i]->name() + " by " + m_list[i]->author());
        }
    }
}
