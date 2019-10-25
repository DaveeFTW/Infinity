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

#include "numberanimation.h"
#include "timer.h"

NumberAnimation::NumberAnimation(float start, float end, int duration)
    : m_start(start)
    , m_end(end)
    , m_duration(duration)
    , m_handler(nullptr)
{
    // default to linear transform
    m_easing = [](float t, float b, float c, float d) { return c * t / d + b; };
}

void NumberAnimation::setEasing(AnimationTransform easing)
{
    m_easing = easing;
}

void NumberAnimation::setStart(float start)
{
    m_start = start;
}

float NumberAnimation::start(void) const
{
    return m_start;
}

void NumberAnimation::setEnd(float end)
{
    m_end = end;
}

float NumberAnimation::end(void) const
{
    return m_end;
}

void NumberAnimation::setDuration(int duration)
{
    m_duration = duration;
}

int NumberAnimation::duration(void) const
{
    return m_duration;
}

void NumberAnimation::start(void)
{
    m_elapsed = 0.f;

    // get a timer listener
    Timer::listen([this](float dt) {
        if (m_elapsed >= m_duration / 1000.f)
        {
            if (m_handler != nullptr)
                m_handler(m_end);

            auto it = m_completeHandlers.begin();
            while (it != m_completeHandlers.end())
            {
                if ((*it)())
                {
                    it = m_completeHandlers.erase(it);
                }
                else
                {
                    it++;
                }
            }

            return true;
        }

        if (m_handler != nullptr)
        {
            // calculate number
            float number = m_easing(m_elapsed, m_start, m_end, m_duration / 1000.f);
            m_handler(number);
        }

        m_elapsed += dt;
        return false;
    });
}

void NumberAnimation::setHandler(AnimationHandler handler)
{
    m_handler = handler;
}

void NumberAnimation::onComplete(AnimationCompleteHandler handler)
{
    m_completeHandlers.push_back(handler);
}
