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

#ifndef CREDITSSCREEN_H
#define CREDITSSCREEN_H

#include "stateview.h"

#include <psputils.h>
#include <string>
#include <vector>

class ViewManager;
class BackgroundView;
class TextRenderer;
class NumberAnimation;

class CreditsScreen : public StateView
{
public:
    CreditsScreen(ViewManager* mgr, BackgroundView* bg);

    bool visible(void) const;

    void setOpacity(float opacity) override;
    float opacity(void) override;

protected:
    void update(float dt) override;
    void render(void) override;

    void begin(State* parent) override;
    void end(void) override;

    ViewManager* viewManager(void);

private:
    ViewManager* m_viewManager;
    BackgroundView* m_background;
    bool m_visible;
    float m_t, m_last_created;
    TextRenderer* m_latinText;
    unsigned int m_currentCredits;
    std::vector<std::string> m_credits;
    NumberAnimation *m_fadeOutBackground, *m_fadeInCredits;
    NumberAnimation *m_delayCredits, *m_fadeOutCredits;
    bool m_fadeOutComplete;
    SceKernelUtilsMt19937Context m_ctx;
};

#endif // CREDITSSCREEN_H
