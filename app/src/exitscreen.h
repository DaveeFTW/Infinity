/*

Copyright (C) 2019, David "Davee" Morgan

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

#pragma once

#include "colouredrectangle.h"
#include "stateview.h"

#include <memory>
#include <string>
#include <vector>

class TextRenderer;
class Event;
class ColouredRectangle;
class ViewManager;

class ExitScreen : public StateView
{
public:
    ExitScreen(ViewManager* manager);
    void setOpacity(float opacity) override;
    float opacity(void) override;

protected:
    void update(float dt) override;
    void render(void) override;

    void begin(State* parent) override;
    void end(void) override;

private:
    void onEvent(Event* event);

private:
    ViewManager* m_viewManager;
    TextRenderer* m_latinText;
    std::unique_ptr<ColouredRectangle> m_rectangle;
    float m_opacity;
    float m_t;
};
