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

#ifndef MENU_H
#define MENU_H

#include "eventsource.h"

#include <string>
#include <vector>

class PageView;
class TextRenderer;
class Event;

class Menu : public EventSource
{
public:
    Menu(PageView* parent, std::initializer_list<std::string> l = {});

    void setX(float x);
    float x(void);
    void setY(float y);
    float y(void);

    void addItem(const std::string& item);
    void clear(void);

    void update(float dt);
    void draw(void);

    void setBackgroundSize(float width, float height);
    void drawBackground(bool drawbg);

    EventList processEvents(void);

private:
    void onEvent(Event* event);

private:
    PageView* m_parent;
    std::vector<std::string> m_list;
    int m_selectedItem;
    unsigned int m_selectionColour;
    bool m_selection;
    TextRenderer* m_latinText;
    float m_t, m_x, m_y;
    bool m_drawbg = true;
    float m_bgWidth = 480.f - 72.f;
    float m_bgHeight = 272.f - 45.f;
};

#endif // MENU_H
