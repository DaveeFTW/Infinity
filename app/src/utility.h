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

#include <pspkerneltypes.h>
#include <psputils.h>

#include <array>
#include <string>
#include <vector>

namespace utility
{
    class File
    {
    public:
        enum Mode
        {
            Read,
            Write
        };

    public:
        File() = default;
        ~File();

        File(const File&) = delete;
        File(File&&) = delete;
        File& operator=(const File&) = delete;
        File& operator=(File&&) = delete;

        bool open(const std::string &filename, Mode mode);
        std::pair<int, std::vector<unsigned char>> read(std::size_t size = 0) const;
        int write(const unsigned char *data, std::size_t size);
        template <typename T>
        int write(const T& data)
        {
            return write(data.data(), data.size());
        }

    private:
        SceUID m_fd = -1;
    };

    class SystemLocker
    {
    public:
        SystemLocker();
        ~SystemLocker();
    };

    template <typename T>
    std::vector<unsigned char> SHA1(const T& data)
    {
    	std::vector<unsigned char> hash(20);
    	SceKernelUtilsSha1Context ctx;
    	sceKernelUtilsSha1BlockInit(&ctx);
        sceKernelUtilsSha1BlockUpdate(&ctx, const_cast<unsigned char *>(data.data()), data.size());
        sceKernelUtilsSha1BlockResult(&ctx, hash.data());
        return hash;
    }

    constexpr int updateVersion(void)
    {
        static_assert((INFINITY_VERSION_MAJOR & 0x7F) == INFINITY_VERSION_MAJOR, "Infinity version major number overflow");
        static_assert((INFINITY_VERSION_MINOR & 0xFFF) == INFINITY_VERSION_MINOR, "Infinity version minor number overflow");
        static_assert((INFINITY_VERSION_PATCH & 0xFFF) == INFINITY_VERSION_PATCH, "Infinity version patch number overflow");
        return ((INFINITY_VERSION_MAJOR & 0x7F) << 24) | ((INFINITY_VERSION_MINOR & 0xFFF) << 12) | (INFINITY_VERSION_PATCH & 0xFFF);
    }
}
