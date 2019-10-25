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

#include "viewmanager.h"
#include "application.h"
#include "buttonarbiter.h"
#include "buttontransition.h"
#include "event.h"
#include "graphicsdevice.h"
#include "numberanimation.h"
#include "scene.h"
#include "state.h"
#include "statemachine.h"
#include "timer.h"
#include "view.h"

#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>

#include <algorithm>

#include "vertextype.h"
#include <pspdebug.h>
#include <string.h>

namespace
{
    void sceGumOrtho2(float left, float right, float bottom, float top, float near, float far)
    {
        ScePspFMatrix4* t;
        ScePspFMatrix4 t2;
        t = &t2;
        float dx = right - left, dy = top - bottom, dz = far - near;

        memset(t, 0, sizeof(ScePspFMatrix4));

        t->x.x = 2.0f / dx;
        t->w.x = -(right + left) / dx;
        t->y.y = 2.0f / dy;
        t->w.y = -(top + bottom) / dy;
        t->z.z = -2.0f / dz;
        t->w.z = -(far + near) / dz;
        t->w.w = 1.0f;

        sceGumMultMatrix(t);
    }
}

ViewManager::ViewManager(void)
    : m_graphicsDevice(new GraphicsDevice())
    , m_buttonArbiter(new ButtonArbiter())
    , m_stateMachine(nullptr)
    , m_scene(nullptr)
{
}

void ViewManager::addView(View* view)
{
    m_views.push_back(view);
}

void ViewManager::removeView(View* view)
{
    auto it = std::find(m_views.begin(), m_views.end(), view);

    if (it != m_views.end())
    {
        m_views.erase(it);
    }
}

void ViewManager::setStateMachine(StateMachine* machine)
{
    m_stateMachine = machine;
}

GraphicsDevice* ViewManager::graphics(void)
{
    return m_graphicsDevice;
}

void ViewManager::exec(void)
{
    m_graphicsDevice->initialise();

    Timer* timer = Timer::instance();
    timer->start();

    while (true)
    {
        // process all events
        Application::instance()->processEvents();

        sceGuStart(GU_DIRECT, m_graphicsDevice->displayList());
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        // disable depth test
        sceGuDepthMask(GU_TRUE);

        // setup ortho projection
        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumOrtho2(0, 480, 272, 0, -1, 1);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();

        auto elapsedTime = timer->elapsed(true);

        for (auto it = m_views.begin(); it != m_views.end(); ++it)
        {
            // update our scene
            (*it)->update(elapsedTime);

            sceGumMatrixMode(GU_MODEL);
            sceGumLoadMatrix((*it)->model());
            sceGumUpdateMatrix();

            (*it)->render();
        }

        sceGuFinish();
        sceGuSync(0, 0);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }
}
