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

#include "timer.h"

#include <time.h>

#include <psprtc.h>

Timer* Timer::m_instance{ nullptr };

Timer* Timer::instance(void)
{
    if (Timer::m_instance == nullptr)
        Timer::m_instance = new Timer();

    return Timer::m_instance;
}

void Timer::listen(TimerListener handler)
{
    instance()->m_listeners.push_back(handler);
}

Timer::Timer(void)
{
    m_tickResolution = (float)sceRtcGetTickResolution();
}

void Timer::start(void)
{
    sceRtcGetCurrentTick(&m_time);
}

float Timer::elapsed(bool delta)
{
    u64 curTime, elapsedTime;

    sceRtcGetCurrentTick(&curTime);
    elapsedTime = curTime - m_time;

    if (delta)
    {
        m_time = curTime;
    }

    auto elapsed = elapsedTime / m_tickResolution;

    for (auto it = m_listeners.begin(); it != m_listeners.end();)
    {
        if ((*it)(elapsed))
        {
            it = m_listeners.erase(it);
        }
        else
        {
            it++;
        }
    }

    return elapsed;
}
