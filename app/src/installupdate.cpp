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

#include "installupdate.h"
#include "mutexlocker.h"
#include "updateinformation.h"
#include "updateinformationparser.h"
#include "utility.h"

#include <config.h>
#include <libinfinity.h>
#include <pspmacro.h>

#include <algorithm>
#include <fstream>
#include <unistd.h>
#include <vector>

#include <pspiofilemgr.h>
#include <pspmodulemgr.h>
#include <pspsysmem.h>
#include <pspthreadman.h>
#include <psputils.h>

#include "generated_file_data.h"

#include <vshbridge.h>

namespace kbridge
{
#include <kernel_helper.h>
};

#include <string.h>

namespace
{
    std::pair<bool, std::vector<unsigned char>> readFile(const std::string& filename)
    {
        using namespace utility;
        File file;

        if (!file.open(filename, File::Read))
        {
            return { false, {} };
        }

        auto data = file.read();

        if (data.first < 0)
        {
            return { false, {} };
        }

        return { true, data.second };
    }

    std::pair<bool, std::vector<unsigned char>> readVerifyFile(
        const std::string& filename,
        const std::vector<unsigned char>& hash)
    {
        using namespace utility;
        File file;

        if (!file.open(filename, File::Read))
        {
            return { false, {} };
        }

        auto data = file.read();

        if (data.first < 0)
        {
            return { false, {} };
        }

        if (SHA1(data.second) != hash)
        {
            return { false, {} };
        }

        return { true, data.second };
    }

    template<typename T>
    bool writeVerifyFile(const std::string& filename, const T& data)
    {
        using namespace utility;
        auto hash = SHA1(data);

        // check if it already is upto date
        auto current = readVerifyFile(filename, hash);

        if (current.first)
        {
            // no need to overwrite with same file
            return true;
        }

        // remove the file
        for (auto i = 0; i < 10; ++i)
        {
            if (sceIoRemove(filename.c_str()) >= 0)
            {
                break;
            }
        }

        // attempt to write 10 times
        for (auto i = 0; i < 10; ++i)
        {
            {
                File file;

                if (!file.open(filename, File::Write))
                {
                    continue;
                }

                auto written = file.write(data);

                if (written < 0)
                {
                    continue;
                }
            }

            // try to verify the file we just wrote
            auto verification = readVerifyFile(filename, hash);

            if (verification.first)
            {
                return true;
            }
        }

        return false;
    }

    std::vector<unsigned char> getBootloader(unsigned int devkit)
    {
        if (devkit == 0x06060010)
        {
            return std::vector<unsigned char>(files::bootloader660::data.begin(),
                                              files::bootloader660::data.end());
        }
        else if (devkit == 0x06060110)
        {
            return std::vector<unsigned char>(files::bootloader661::data.begin(),
                                              files::bootloader661::data.end());
        }

        return {};
    }
};

InstallUpdate::InstallUpdate(void)
    : m_state(CHECKING_FILES)
{
    m_eventId = sceKernelCreateEventFlag("infinityInstall", 0, 0, NULL);
}

InstallUpdate::~InstallUpdate(void)
{
    sceKernelDeleteEventFlag(m_eventId);
}

InstallUpdate::State InstallUpdate::state(void) const
{
    return m_state;
}

void InstallUpdate::cancel(void)
{
    MutexLocker locker(&m_mutex);
    sceKernelSetEventFlag(m_eventId, EVENT_CANCEL);
}

void InstallUpdate::install(void)
{
    MutexLocker locker(&m_mutex);
    sceKernelSetEventFlag(m_eventId, EVENT_INSTALL);
}

void InstallUpdate::setState(State state)
{
    MutexLocker locker(&m_mutex);
    m_state = state;
}

void InstallUpdate::run(void)
{
    utility::SystemLocker locker;

    // swap directory
    char cwd[512];
    sceIoChdir(getcwd(cwd, sizeof(cwd)));

    // do install
    setState(INSTALLING);

    if (sceIoUnassign("flash0:") < 0)
    {
        setState(ERROR_UNASSIGN_FLASH0);
        return;
    }

    if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
    {
        setState(ERROR_ASSIGN_FLASH0);
        return;
    }

    auto notInstalled = infGetVersion() < 0;

    if (notInstalled)
    {
        auto usl = readFile("flash0:/kd/usersystemlib.prx");

        if (!usl.first)
        {
            setState(ERROR_READ_USERSYSTEMLIB);
            return;
        }

        if (!writeVerifyFile("flash0:/kd/usersystemlib.inf", usl.second))
        {
            setState(ERROR_WRITE_BACKUP);
            return;
        }

        sceIoRemove(INFINITY_CONFIGURATION_LOCATION);
    }

    if (!writeVerifyFile(INFINITYBOOT_EXEC_PATH, files::infinityboot::data))
    {
        setState(ERROR_WRITE_INFINITYBOOT);
        return;
    }

    std::vector<unsigned char> infinityctrl(files::infinityctrl::data.begin(),
                                            files::infinityctrl::data.end());
    auto res = kbridge::pspSignCheck(infinityctrl.data());

    if (res < 0)
    {
        setState(ERROR_APPLYING_SIGNCHECK);
        return;
    }

    if (!writeVerifyFile(INFINITYCTRL_EXEC_PATH, infinityctrl))
    {
        setState(ERROR_WRITE_INFINITYCTRL);
        return;
    }

    if (!writeVerifyFile(REBOOTEX_EXEC_PATH, files::rebootex::data))
    {
        setState(ERROR_WRITE_REBOOTEX);
        return;
    }

    if (!writeVerifyFile(MECOMPAT_EXEC_PATH, files::mecompat::data))
    {
        setState(ERROR_WRITE_MECOMPAT);
        return;
    }

    if (!writeVerifyFile(PROCOMPAT_EXEC_PATH, files::procompat::data))
    {
        setState(ERROR_WRITE_PROCOMPAT);
        return;
    }
    
    if (!writeVerifyFile(ARKCOMPAT_EXEC_PATH, files::arkcompat::data))
    {
        setState(ERROR_WRITE_ARKCOMPAT);
        return;
    }

    auto bootloader = getBootloader(sceKernelDevkitVersion());

    if (bootloader.empty())
    {
        setState(ERROR_INVALID_BOOTLOADER);
        return;
    }

    res = kbridge::pspSignCheck(bootloader.data());

    if (res < 0)
    {
        setState(ERROR_APPLYING_SIGNCHECK);
        return;
    }

    if (!writeVerifyFile("flash0:/kd/usersystemlib.prx", bootloader))
    {
        setState(ERROR_READ_USERSYSTEMLIB);
        return;
    }

    infSetRedirectionStatus(1);
    setState(INSTALL_COMPLETE);
}
