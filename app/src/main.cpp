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

#include "animations.h"
#include "backgroundview.h"
#include "buttontransition.h"
#include "colouredrectangle.h"
#include "configscreen.h"
#include "creditsscreen.h"
#include "errorscreen.h"
#include "font2.h"
#include "fontmanager.h"
#include "homescreen.h"
#include "infinityintro.h"
#include "installscreen.h"
#include "intraFont.h"
#include "numberanimation.h"
#include "pageview.h"
#include "particlesource.h"
#include "statemachine.h"
#include "textrenderer.h"
#include "tgatexture.h"
#include "uptodatescreen.h"
#include "utility.h"
#include "viewmanager.h"

#include "generated_file_data.h"

#include <kexploit.h>
#include <libinfinity.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspsdk.h>

#include "vertextype.h"
#include <pspgu.h>
#include <pspgum.h>

#include <pspmacro.h>
#include <vshbridge.h>

#include <iostream>
#include <sstream>
#include <string>

PSP_MODULE_INFO(infinity_app, 0x800, 1, 0);
PSP_HEAP_SIZE_KB(-2048);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

namespace
{
    void addSmoothParallaxTransitionLeft(auto source, auto dest, auto parallax)
    {
        auto transition = new ButtonTransition(PSP_CTRL_LEFT);
        auto animation = std::make_shared<NumberAnimation>(0, 480);

        auto easing = [](float t, float b, float c, float d) {
            t /= d;
            return -c * t * (t - 2) + b;
        };

        animation->setHandler([source, dest, easing](float number) {
            auto newval = easing(number, 0.f, 480.f, 480.f);

            ScePspFVector3 moveOut = { newval, 0, 0 };
            ScePspFVector3 moveIn = { -(480 - newval), 0, 0 };

            auto model = source->model();
            gumLoadIdentity(model);
            gumTranslate(model, &moveOut);

            model = dest->model();
            gumLoadIdentity(model);
            gumTranslate(model, &moveIn);
        });

        auto parallaxAnimation = std::make_shared<NumberAnimation>(0, 100);

        parallaxAnimation->setHandler([parallax, easing](float number) {
            auto newval = easing(number, 0.f, 100.f, 100.f);

            ScePspFVector3 moveOut = { newval, 0, 0 };
            auto model = parallax->model();
            gumLoadMatrix(model, parallax->loadModel());
            gumTranslate(model, &moveOut);
        });

        parallaxAnimation->onComplete([parallax]() {
            ScePspFVector3 moveOut = { 100, 0, 0 };
            auto model = parallax->model();
            gumLoadMatrix(model, parallax->loadModel());
            gumTranslate(model, &moveOut);
            parallax->saveModel(model);
            return false;
        });

        transition->addAnimation(animation);
        transition->addAnimation(parallaxAnimation);
        source->addTransition(transition, dest);
        source->setPageLeft(true);
    }

    void addSmoothParallaxTransitionRight(auto source, auto dest, auto parallax)
    {
        auto transition = new ButtonTransition(PSP_CTRL_RIGHT);
        auto animation = std::make_shared<NumberAnimation>(0, 480);

        auto easing = [](float t, float b, float c, float d) {
            t /= d;
            return -c * t * (t - 2) + b;
        };

        animation->setHandler([source, dest, easing](float number) {
            auto newval = easing(number, 0.f, 480.f, 480.f);

            ScePspFVector3 moveOut = { -newval, 0, 0 };
            ScePspFVector3 moveIn = { (480 - newval), 0, 0 };

            auto model = source->model();
            gumLoadIdentity(model);
            gumTranslate(model, &moveOut);

            model = dest->model();
            gumLoadIdentity(model);
            gumTranslate(model, &moveIn);
        });

        auto parallaxAnimation = std::make_shared<NumberAnimation>(0, 100);

        parallaxAnimation->setHandler([parallax, easing](float number) {
            auto newval = easing(number, 0.f, 100.f, 100.f);

            ScePspFVector3 moveOut = { -newval, 0, 0 };
            auto model = parallax->model();
            gumLoadMatrix(model, parallax->loadModel());
            gumTranslate(model, &moveOut);
        });

        parallaxAnimation->onComplete([parallax]() {
            ScePspFVector3 moveOut = { -100, 0, 0 };
            auto model = parallax->model();
            gumLoadMatrix(model, parallax->loadModel());
            gumTranslate(model, &moveOut);
            parallax->saveModel(model);
            return false;
        });

        transition->addAnimation(animation);
        transition->addAnimation(parallaxAnimation);
        source->addTransition(transition, dest);
        source->setPageRight(true);
    }

    class TestScreen : public PageView
    {
    public:
        TestScreen(ViewManager* viewManager, unsigned int color)
            : PageView(viewManager, "Test Screen")
            , m_opacity(1.f)
            , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
        {
            m_color = color;
            m_latinText->font()->setColour(Font::WHITE);

            std::stringstream ss;
            ss << "Infinity Internal Version: R" << std::hex << infGetVersion() << " CORE: RBAD";
            m_version = ss.str();
        }

        virtual void render(void)
        {
            sceGuDisable(GU_TEXTURE_2D);

            Vertex* triangle = static_cast<Vertex*>(sceGuGetMemory(sizeof(Vertex) * 2));

            triangle[0].colour = m_color;
            triangle[0].x = 240 - 20;
            triangle[0].y = 136 - 20.f;
            triangle[0].z = 0.0f;
            triangle[1].colour = m_color;
            triangle[1].x = 240 + 20.f;
            triangle[1].y = 136 + 20.f;
            triangle[1].z = 0.0f;

            sceGumDrawArray(
                GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2, 0, triangle);
            PageView::render();
        }

        virtual void update(float t) { PageView::update(t); }

        virtual void setOpacity(float opacity) override
        {
            m_opacity = opacity;
            int alpha = 0xFF * opacity;

            if (alpha > 0xFF)
                alpha = 0xFF;
            if (alpha < 0)
                alpha = 0;

            m_color &= 0xFFFFFF;
            m_color |= alpha << 24;
            m_latinText->font()->setOpacity(opacity);
        }

        float opacity(void) override { return m_opacity; }

    private:
        unsigned int m_color;
        float m_opacity;
        TextRenderer* m_latinText;
        std::string m_version;
    };

    void disableKernelModuleWhitelist(const KernelContext* ctx)
    {
        auto baseAddress = ctx->pspKernelFindModuleByName("sceMemlmd")->text_addr;

        // force whitelist check to return 1
        MAKE_FUNCTION_RETURN(baseAddress + 0xA8, 1); // 6.60/6.61
        ctx->clearCaches();
    }
}

int main(int argc, char* argv[])
{
    intraFontInit();
    ViewManager* manager = new ViewManager();

    sceKernelDelayThread(5000);

    // disable executable whitelist
    auto res = executeInKernelMode(disableKernelModuleWhitelist);

    if (res < 0)
    {
        manager->addView(new ErrorScreen("Could not disable kernel whitelist", res));
        manager->exec();
        return 0;
    }

    SceKernelLMOption opt;
    memset(&opt, 0, sizeof(opt));

    opt.size = sizeof(opt);
    opt.mpidtext = 1;
    opt.mpiddata = 1;
    opt.position = 0;
    opt.access = 1;

    // load libconfig
    SceUID modid = vshKernelLoadModuleBufferVSH(
        files::libconfig::data.size(), files::libconfig::data.data(), 0, &opt);

    if (modid < 0)
    {
        manager->addView(new ErrorScreen("Could not load libconfig", modid));
        manager->exec();
        return 0;
    }

    res = sceKernelStartModule(modid, 0, nullptr, nullptr, nullptr);

    if (res < 0)
    {
        manager->addView(new ErrorScreen("Could not start libconfig", res));
        manager->exec();
        return 0;
    }

    memset(&opt, 0, sizeof(opt));

    opt.size = sizeof(opt);
    opt.mpidtext = 1;
    opt.mpiddata = 1;
    opt.position = 0;
    opt.access = 1;

    // load kernel helper
    modid = vshKernelLoadModuleBufferVSH(
        files::kernel_helper::data.size(), files::kernel_helper::data.data(), 0, &opt);

    if (modid < 0)
    {
        manager->addView(new ErrorScreen("Could not load kernel_helper", modid));
        manager->exec();
        return 0;
    }

    res = sceKernelStartModule(modid, 0, nullptr, nullptr, nullptr);

    if (res < 0)
    {
        manager->addView(new ErrorScreen("Could not start kernel_helper", res));
        manager->exec();
        return 0;
    }

    // check firmware version is 6.60 or 6.61
    if (sceKernelDevkitVersion() != 0x06060010 && sceKernelDevkitVersion() != 0x06060110)
    {
        manager->addView(new ErrorScreen("Infinity only runs on 6.60 or 6.61. Please update."));
        manager->exec();
        return 0;
    }

    // check for old infinity version
    // old infinity version was just the git commit count. The version is < 0xFF
    if (!(infGetVersion() & ~0xFF))
    {
        manager->addView(new ErrorScreen(
            "You're running Infinity 1.X.\nPlease uninstall with chronoswitch 7.0+"));
        manager->exec();
        return 0;
    }

    auto homeScreen = std::make_shared<HomeScreen>(manager);
    auto configScreen = std::make_shared<ConfigScreen>(manager);
    auto installScreen = std::make_shared<InstallScreen>(manager);
    auto uptoDateScreen = std::make_shared<UptoDateScreen>(manager);
    auto parallax = new BackgroundView(manager);
    auto creditsScreen = std::make_shared<CreditsScreen>(manager, parallax);

    parallax->setTextureLeft(
        new TgaTexture(files::parallax_left::data.data(), files::parallax_left::data.size()));
    parallax->setTextureRight(
        new TgaTexture(files::parallax_right::data.data(), files::parallax_right::data.size()));

    manager->addView(parallax);

    // check if we're doing an install or just configuration
    auto update_available = infGetVersion() < utility::updateVersion();
    auto not_installed = infGetVersion() < 0;

    // add smooth transitions
    addSmoothParallaxTransitionLeft(homeScreen.get(), configScreen.get(), parallax);
    addSmoothParallaxTransitionRight(configScreen.get(), homeScreen.get(), parallax);

    if (!update_available)
    {
        addSmoothParallaxTransitionLeft(uptoDateScreen.get(), homeScreen.get(), parallax);
        addSmoothParallaxTransitionRight(homeScreen.get(), uptoDateScreen.get(), parallax);
    }

    else if (!not_installed)
    {
        addSmoothParallaxTransitionLeft(installScreen.get(), homeScreen.get(), parallax);
        addSmoothParallaxTransitionRight(homeScreen.get(), installScreen.get(), parallax);
    }

    ButtonTransition* btn = new ButtonTransition(PSP_CTRL_SELECT);
    smoothFadeOutAnimation(homeScreen.get(), creditsScreen.get(), btn);
    ButtonTransition* rt = new ButtonTransition(PSP_CTRL_RTRIGGER);
    smoothFadeOutAnimation(creditsScreen.get(), homeScreen.get(), rt);

    // add state machine
    StateMachine machine;
    machine.addState(homeScreen);
    machine.addState(configScreen);
    machine.addState(installScreen);
    machine.addState(uptoDateScreen);

    if (update_available && not_installed)
    {
        machine.setInitialState(installScreen);
    }
    else
    {
        machine.setInitialState(homeScreen);
    }

    machine.start();

    manager->setStateMachine(&machine);
    manager->exec();
    return 0;
}
