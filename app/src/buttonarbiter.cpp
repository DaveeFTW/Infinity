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

#include "buttonarbiter.h"
#include "buttonevent.h"
#include "eventhandler.h"

#include <vshbridge.h>

#include <pspctrl.h>
#include <psptypes.h>

#include <algorithm>

ButtonArbiter::ButtonArbiter(void)
    : m_previousButtons(0)
{
}

unsigned int ButtonArbiter::buttons(void)
{
    SceCtrlData pad;
    vshCtrlReadBufferPositive(&pad, 1);

    auto buttons = pad.Buttons & ~m_previousButtons;
    m_previousButtons = pad.Buttons;
    return buttons;
}

void ButtonArbiter::bind(unsigned int buttons, ButtonHandler handler)
{
    m_handlers.push_back(std::make_pair(buttons, handler));
}

void ButtonArbiter::reset(void)
{
    m_handlers.clear();
}

EventList ButtonArbiter::processEvents(void)
{
    EventList list;

    auto pressed = buttons();

    if (pressed != 0)
    {
        // handler(new ButtonEvent(pressed));
        list.push_back(new ButtonEvent(pressed));
    }

    return list;
}
