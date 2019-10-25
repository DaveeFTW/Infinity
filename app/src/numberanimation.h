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

#ifndef NUMBERANIMATION_H
#define NUMBERANIMATION_H

#include "animation.h"

#include <functional>
#include <vector>

class NumberAnimation : public Animation
{
public:
    using AnimationHandler = std::function<void(float num)>;
    using AnimationTransform = std::function<float(float, float, float, float)>;
    using AnimationCompleteHandlerList = std::vector<AnimationCompleteHandler>;

public:
    NumberAnimation(float start, float end, int duration = 200);

    void setEasing(AnimationTransform easing);

    void setStart(float start);
    float start(void) const;

    void setEnd(float end);
    float end(void) const;

    void setDuration(int duration);
    int duration(void) const;

    void setHandler(AnimationHandler handler);

    void start(void) override;
    void onComplete(AnimationCompleteHandler handler) override;

private:
    float m_start, m_end, m_elapsed;
    int m_duration;

    AnimationHandler m_handler;
    AnimationCompleteHandlerList m_completeHandlers;
    AnimationTransform m_easing;
};

#endif // ANIMATION_H
