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

#include "installupdatescreen.h"
#include "animations.h"
#include "application.h"
#include "buttonevent.h"
#include "font2.h"
#include "installupdate.h"
#include "textrenderer.h"

#include <kernel_helper.h>
#include <libinfinity.h>

#include <cmath>
#include <iostream>
#include <sstream>

#include <pspctrl.h>
#include <pspgu.h>
#include <pspiofilemgr.h>
#include <psploadexec.h>

#include "vertextype.h"
#include <pspgum.h>

#include "buttontransition.h"
#include "functortransition.h"
#include "numberanimation.h"

extern "C"
{
    int scePowerRequestColdReset(int unk);
}

InstallUpdateScreen::InstallUpdateScreen(ViewManager* viewManager, PageView* parent)
    : PageView(viewManager, "Install Update", false)
    , m_parentView(parent)
    , m_latinText(new TextRenderer(new Font("flash0:/font/ltn0.pgf")))
    , m_opacity(1.0f)
    , m_t(0.f)
    , m_state(CHECKING_CONTENTS)
    , m_worker(nullptr)
{
    Application::instance()->addEventHandler([=](Event* event) {
        if (visible())
        {
            onEvent(event);
        }
    });

    auto returnTransition = new FunctorTransition(
        [=](void) {
            if (!m_worker)
            {
                return false;
            }

            if (m_worker->state() == InstallUpdate::ERROR_FLASHING_BOOTLOADER ||
                m_worker->state() == InstallUpdate::ERROR_OPENING_CORE_FLASH0 ||
                m_worker->state() == InstallUpdate::ERROR_FLASHING_CORE_FLASH0 ||
                m_worker->state() == InstallUpdate::ERROR_CLOSING_CORE_FLASH0 ||
                m_worker->state() == InstallUpdate::ERROR_OPENING_SUBSET_FLASH0 ||
                m_worker->state() == InstallUpdate::ERROR_FLASHING_SUBSET_FLASH0 ||
                m_worker->state() == InstallUpdate::ERROR_CLOSING_SUBSET_FLASH0)
            {
                sceKernelExitGame();
            }

            return m_worker->state() == InstallUpdate::UPDATE_UNAVAILABLE ||
                   m_worker->state() == InstallUpdate::INVALID_UPDATE ||
                   m_worker->state() == InstallUpdate::CONTAINER_WRITE_FAILURE ||
                   m_worker->state() == InstallUpdate::CONTAINER_OPEN_FAILURE ||
                   m_worker->state() == InstallUpdate::NO_UPDATER_VERSION ||
                   m_worker->state() == InstallUpdate::NO_INTERNAL_VERSION ||
                   m_worker->state() == InstallUpdate::NO_UPDATE_REQUIRED ||
                   m_worker->state() == InstallUpdate::MISSING_631_OFW ||
                   m_worker->state() == InstallUpdate::NO_UTILITIES ||
                   m_worker->state() == InstallUpdate::CANNOT_LOAD_UTILITY ||
                   m_worker->state() == InstallUpdate::CANNOT_START_UTILITY;
        },
        std::make_shared<ButtonTransition>(PSP_CTRL_CIRCLE));

    smoothFadeOutAnimation(this, parent, returnTransition);
}

void InstallUpdateScreen::setOpacity(float opacity)
{
    m_opacity = opacity;
    m_latinText->font()->setOpacity(m_opacity);
}

float InstallUpdateScreen::opacity(void)
{
    return m_opacity;
}

void InstallUpdateScreen::update(float dt)
{
    PageView::update(dt);
}

void InstallUpdateScreen::render(void)
{
    switch (m_worker->state())
    {
        case InstallUpdate::CHECKING_FILES:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Checking files...", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_BACKUP:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Could not backup opening_plugin.rco.",
                                         TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_INFINITYBOOT:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not flash infinityboot.prx.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_INFINITYCTRL:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not flash infinityctrl.prx.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_REBOOTEX:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not flash rebootex.bin.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_MECOMPAT:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not flash mecompat.bin.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_PROCOMPAT:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not flash procompat.bin.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_ARKCOMPAT:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not flash arkcompat.bin.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_READ_USERSYSTEMLIB:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Could not read usersystemlib.prx from flash0.",
                                         TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         80.f,
                                         "Infinity was not installed on your device.\nPlease exit "
                                         "and reflash 6.61 before installing.",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_WRITE_USERSYSTEMLIB:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Could not flash usersystemlib.prx.",
                                         TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f,
                80.f,
                "This is a fatal error and your device may be bricked.\nPlease exit and reflash "
                "6.61 before installing.",
                TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::UPDATE_UNAVAILABLE:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not open update file.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::INVALID_UPDATE:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Update file is corrupt or invalid.",
                                         TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::CONTAINER_WRITE_FAILURE:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not write container file.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::CONTAINER_OPEN_FAILURE:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not open container file.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::NO_UPDATER_VERSION:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Update file has no version.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::NO_INTERNAL_VERSION:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Update has no internal version definition.",
                                         TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::NO_UPDATE_REQUIRED:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Already up-to date.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::CHECKING_631_OFW:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Checking 6.31 Official Firmware...",
                                         TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::MISSING_631_OFW:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "631.PBP is missing or invalid.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::INSTALLING:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Installing...", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Please wait...", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::NO_UTILITIES:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Update has no UTILITY section.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::CANNOT_LOAD_UTILITY:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not load updater modules.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::CANNOT_START_UTILITY:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Could not start updater modules.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_UNASSIGN_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Failed to unassign flash0.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_ASSIGN_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Failed to reassign flash0 as RW.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::FLASHING_BOOTLOADER:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Signing and flashing bootloader...",
                                         TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Please wait...", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_MOUNTING_631:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error mounting 6.31 updater...", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_UNMOUNTING_631:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error unmounting 6.31 updater...", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_READING_SYSTIMER_PRX:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error reading 6.31 systimer.prx!", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_READING_INIT_PRX:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error reading 6.31 init.prx!", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to return.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_FLASHING_BOOTLOADER:
            m_latinText->drawVerticalTop(
                480.f / 2.f,
                50.f,
                "Critical error flashing bootloader!\nYour system may be in jeopardy.\nDo NOT turn "
                "off your system.\nUse chronoswitch or reflash infinity.",
                TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 110.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::FLASHING_CORE_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Flashing CORE flash0 files...", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Please wait...", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_OPENING_CORE_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error opening CORE flash0 files.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_FLASHING_CORE_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error flashing CORE flash0 files.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_CLOSING_CORE_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error closing CORE flash0 files.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::FLASHING_SUBSET_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Flashing 6.61 flash0 files...", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Please wait...", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_OPENING_SUBSET_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error opening 6.61 flash0 files.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_FLASHING_SUBSET_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error flashing 6.61 flash0 files.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_CLOSING_SUBSET_FLASH0:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Error closing 6.61 flash0 files.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::INSTALL_COMPLETE:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "Installation complete.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press X to reboot.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_APPLYING_SIGNCHECK:
            m_latinText->drawVerticalTop(480.f / 2.f,
                                         50.f,
                                         "Error applying signcheck to bootloader.",
                                         TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
        case InstallUpdate::ERROR_INVALID_BOOTLOADER:
            m_latinText->drawVerticalTop(
                480.f / 2.f, 50.f, "No valid firmware for bootloader.", TextRenderer::ALIGN_CENTER);
            m_latinText->drawVerticalTop(
                480.f / 2.f, 80.f, "Press O to QUIT.", TextRenderer::ALIGN_CENTER);
            break;
    }

    PageView::render();
}

void InstallUpdateScreen::onEvent(Event* event)
{
    if (event->type() != Event::Button)
        return;

    ButtonEvent* buttonEvent = static_cast<ButtonEvent*>(event);

    // check if install has been completed
    if (m_worker->state() == InstallUpdate::INSTALL_COMPLETE &&
        buttonEvent->buttons() == PSP_CTRL_CROSS)
    {
        // begin install
        scePowerRequestColdReset(0);
    }
}

void InstallUpdateScreen::begin(State* parent)
{
    PageView::begin(parent);
    m_state = CHECKING_CONTENTS;
    m_worker = new InstallUpdate();
    m_worker->start();
}

void InstallUpdateScreen::end(void)
{
    delete m_worker;
    m_worker = nullptr;
    PageView::end();
}
