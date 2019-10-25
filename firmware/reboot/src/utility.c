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

#include "utility.h"

#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <pspsdk.h>

int FileSize(const char* file)
{
    SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

    if (fd < 0)
        return fd;

    int seek = sceIoLseek(fd, 0, PSP_SEEK_END);
    sceIoClose(fd);
    return seek;
}

int ReadFile(const char* file, int seek, void* buf, int size)
{
    SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
    if (fd < 0)
        return fd;

    if (seek > 0)
    {
        if (sceIoLseek(fd, seek, PSP_SEEK_SET) != seek)
        {
            sceIoClose(fd);
            return -1;
        }
    }

    int read = sceIoRead(fd, buf, size);

    sceIoClose(fd);
    return read;
}

void ClearCaches(void)
{
    sceKernelIcacheInvalidateAll();
    sceKernelDcacheWritebackInvalidateAll();
}
