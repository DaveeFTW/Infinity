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

#ifndef INSTALLUPDATESCREEN_H
#define INSTALLUPDATESCREEN_H

#include "pageview.h"

#include <memory>

class TextRenderer;
class Event;
class InstallUpdate;

class InstallUpdateScreen : public PageView
{
public:
    InstallUpdateScreen(ViewManager* viewManager, PageView* parent);

    void setOpacity(float opacity) override;
    float opacity(void) override;

protected:
    void update(float dt) override;
    void render(void) override;

private:
    void onEvent(Event* event);
    void begin(State* parent) override;
    void end(void) override;

private:
    enum UpdateStates
    {
        CHECKING_CONTENTS,
        OFFICIAL_UPDATE,
        INVALID_UPDATE,
        UNOFFICIAL_UPDATE
    };

private:
    PageView* m_parentView;
    TextRenderer* m_latinText;
    float m_opacity;
    float m_t;
    InstallUpdateScreen::UpdateStates m_state;
    InstallUpdate* m_worker;
};

#endif // INSTALLUPDATESCREEN_H
