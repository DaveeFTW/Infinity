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

#include "application.h"
#include <algorithm>

Application* Application::m_instance = nullptr;

Application* Application::instance(void)
{
    if (m_instance == nullptr)
    {
        m_instance = new Application();
    }

    return m_instance;
}

Application::Application(void) {}

void Application::submitEvent(Event* event)
{
    std::for_each(m_handlers.begin(), m_handlers.end(), [event](auto handler) { handler(event); });
}

void Application::addEventHandler(EventHandler handler)
{
    m_handlers.push_back(handler);
}

void Application::addEventSource(EventSource* source)
{
    m_sources.push_back(source);
}

void Application::processEvents(void)
{
    for (auto source = m_sources.begin(); source != m_sources.end(); ++source)
    {
        auto events = (*source)->processEvents();

        for (auto it = events.begin(); it != events.end(); ++it)
        {
            submitEvent(*it);
        }
    }
}
