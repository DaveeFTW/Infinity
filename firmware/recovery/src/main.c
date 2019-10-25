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

#include <pspiofilemgr.h>
#include <pspmacro.h>
#include <string.h>

char data[32 * 1024] = { 1 };
uintptr_t g_base = -1;

int thishelper(uintptr_t helper)
{
    int (*getThreadId)() = (void*)(g_base + 0xA94);

    for (int i = 0; i < 1000000; ++i)
    {
        getThreadId();
    }

    return 0;
}

void recovery_entry(uintptr_t module_start)
{
    uintptr_t text_addr = module_start - 0x838;
    void (*clearcaches)() = (void*)(text_addr + 0x1350);

    _sh(0x3000, text_addr + 0x39C);
    clearcaches();
    /*
    while (1)
    {
    }

    clearcaches();
    int (*powerlock)(int lock) = NULL;
    int (*open)(const char* file, int, int) = NULL;
    int (*write)(int fd, uintptr_t address, int) = NULL;
    int (*close)(int fd) = NULL;
    _sh(0x40, module_start + 0x440 - 0xB8);

    open = (void*)(module_start + 0xA24 - 0xB8);
    write = (void*)(module_start + 0xA34 - 0xB8);
    close = (void*)(module_start + 0xA1C - 0xB8);
    powerlock = (void*)(module_start - 0xB8 + 0xA54);
    g_base = module_start - 0xB8;

    // int fd = open("ms0:/rec_out.bin", PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

    // write(fd, module_start + 0x1134 - 0xB8, 0x20);
    // close(fd);

    MAKE_CALL(module_start + 0x188 - 0xB8, thishelper);

    clearcaches();
    // while (1)
    // {
    //     powerlock(0);
    // }*/
}
