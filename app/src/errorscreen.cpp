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

#include "errorscreen.h"
#include "font2.h"
#include "textrenderer.h"

#include <iostream>
#include <sstream>

ErrorScreen::ErrorScreen(const std::string& msg)
    : m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_opacity(1.0f)
{
    m_errorMessage = msg;
}

ErrorScreen::ErrorScreen(const std::string& msg, unsigned int errorcode)
    : m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_opacity(1.0f)
{
    std::stringstream e;
    e << msg << " (" << std::hex << errorcode << ")";
    m_errorMessage = e.str();
}

void ErrorScreen::setOpacity(float opacity)
{
    m_opacity = opacity;
    m_latinText->font()->setOpacity(opacity);
}

float ErrorScreen::opacity(void)
{
    return m_opacity;
}

void ErrorScreen::update(float dt) {}

void ErrorScreen::render(void)
{
    m_latinText->draw(480.f / 2.f, 272.f / 2.f, m_errorMessage, TextRenderer::ALIGN_CENTER);
}
