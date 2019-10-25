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

#ifndef INITSTATE_H_
#define INITSTATE_H_

#include "errorstate.h"
#include "mainmenustate.h"
#include "state.h"

#include <config.h>

#include <pspkernel.h>

#include <memory>
#include <stdio.h>
class InitState : public State
{
public:
    InitState(ConfigurationMenu* ctx)
        : State(ctx)
    {
    }

    void begin(StatePtr caller) override
    {
        // load libconfig
        SceUID modid = sceKernelLoadModule("libconfig.prx", 0, NULL);

        if (modid < 0)
        {
            context()->setState(
                std::make_shared<ErrorState>(context(), "Error loading libconfig.prx."));
            return;
        }

        int res = sceKernelStartModule(modid, 0, NULL, NULL, NULL);

        if (res < 0)
        {
            context()->setState(
                std::make_shared<ErrorState>(context(), "Error starting libconfig.prx."));
            return;
        }

        // load the infinity config
        if ((res = infInitConfig(INFINITY_CONFIGURATION_LOCATION)) < 0)
        {
            // could not load, try to create it instead
            if ((res = infCreateConfig(INFINITY_CONFIGURATION_LOCATION)) < 0)
            {
                // error!
                context()->setState(std::make_shared<ErrorState>(
                    context(), "Error creating infinity configuration."));
            }
            else
            {
                // first run
                // context()->setState(std::make_shared<FirstRunState>(context()));
            }
        }
        else
        {
            // go to splash
        }
        context()->setState(std::make_shared<MainMenuState>(context()));
    }

    void end(void) override {}

    Scene* scene(void) override { return nullptr; }
};

#endif // INITSTATE_H_
