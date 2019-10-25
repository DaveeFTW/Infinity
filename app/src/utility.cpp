#include "utility.h"

#include <kernel_helper.h>

#include <pspiofilemgr.h>

extern "C" int sceKernelPowerLock(int timers);
extern "C" int sceKernelPowerUnlock(int timers);

namespace utility
{
    namespace
    {
        auto mode_to_sce_mode(File::Mode mode)
        {
            switch (mode)
            {
                default:
                case File::Read:
                    return PSP_O_RDONLY;
                case File::Write:
                    return PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC;
            }
        }
    }

    File::~File()
    {
        if (m_fd > 0)
        {
            sceIoClose(m_fd);
        }
    }

    bool File::open(const std::string& filename, Mode mode)
    {
        auto sce_mode = mode_to_sce_mode(mode);
        auto fd = sceIoOpen(filename.c_str(), sce_mode, 0777);

        if (fd < 0)
        {
            return false;
        }

        m_fd = fd;
        return true;
    }

    std::pair<int, std::vector<unsigned char>> File::read(std::size_t size) const
    {
        std::vector<unsigned char> data;

        if (size == 0)
        {
            auto current = sceIoLseek(m_fd, 0, PSP_SEEK_CUR);
            size = sceIoLseek(m_fd, 0, PSP_SEEK_END);
            sceIoLseek(m_fd, current, PSP_SEEK_SET);
        }

        data.resize(size);

        auto res = sceIoRead(m_fd, data.data(), size);
        return { res, data };
    }

    int File::write(const unsigned char* data, std::size_t size)
    {
        return sceIoWrite(m_fd, data, size);
    }

    SystemLocker::SystemLocker()
    {
        sceKernelPowerLock(0);
        setHomePopup(0);
    }

    SystemLocker::~SystemLocker()
    {
        setHomePopup(1);
        sceKernelPowerUnlock(0);
    }
}
