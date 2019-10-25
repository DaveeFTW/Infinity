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

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "view.h"
#include <vector>

using ViewList = std::vector<View*>;

class GraphicsDevice;
class ButtonArbiter;
class StateMachine;
class Scene;

class ViewManager
{
public:
    ViewManager(void);

    void addView(View* view);
    void removeView(View* view);

    void setStateMachine(StateMachine* machine);

    GraphicsDevice* graphics(void);

    void exec(void);

private:
    GraphicsDevice* m_graphicsDevice;
    ButtonArbiter* m_buttonArbiter;
    StateMachine* m_stateMachine;
    Scene* m_scene;
    ViewList m_views;
};

#endif // VIEWMANAGER_H
