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

#include "lfatpatch.h"

#include <pspmacro.h>

#include <string.h>

// lfat functions
int (*sceBootLfatOpen)(char* path) = NULL;

#ifdef MS0_LOAD
const char* g_infinityctrl;
int g_infinityctrl_size;
int g_lfat_open = 0;

int (*sceBootLfatRead)(void* data, int size) = NULL;
int (*sceBootLfatClose)(void) = NULL;
#endif

int sceBootLfatOpenPatched(char* path)
{
#ifdef MS0_LOAD
    if (memcmp(path, "/kn/infinityctrl.prx", strlen("/kn/infinityctrl.prx")) == 0)
    {
        g_lfat_open = 1;
        return 0;
    }
    else
    {
        g_lfat_open = 0;
    }
#endif

    return sceBootLfatOpen(path);
}

#ifdef MS0_LOAD
int sceBootLfatReadPatched(void* data, int size)
{
    if (g_lfat_open)
    {
        memmove(data, g_infinityctrl, g_infinityctrl_size);
        return g_infinityctrl_size;
    }

    return sceBootLfatRead(data, size);
}

int sceBootLfatClosePatched(void)
{
    if (g_lfat_open)
    {
        g_lfat_open = 0;
        return 0;
    }

    return sceBootLfatClose();
}

void setInfinityctrl(const char* data, int size)
{
    g_infinityctrl = data;
    g_infinityctrl_size = size;
}
#endif

void applyLfatPatches(int model)
{
    switch (model)
    {
        case PSP_MODEL_PHAT:
        {
            // extract from JAL to preserve patch location
            sceBootLfatOpen = (void*)EXTRACT_J_ADDR(0x886027C4); // 6.61

            // patch io to redirect
            MAKE_CALL(0x886027C4, sceBootLfatOpenPatched); // 6.61
#ifdef MS0_LOAD
            // extract from JAL to preserve patch location
            sceBootLfatRead = (void*)EXTRACT_J_ADDR(0x88602834);  // 6.61
            sceBootLfatClose = (void*)EXTRACT_J_ADDR(0x88602860); // 6.61

            MAKE_CALL(0x88602834, sceBootLfatReadPatched);  // 6.61
            MAKE_CALL(0x88602860, sceBootLfatClosePatched); // 6.61

            // prevent lseek errors
            _sw(0x00000000, 0x88602810); // 6.61
            _sw(0x00000000, 0x88602828); // 6.61
#endif
            break;
        }

        case PSP_MODEL_SLIM:
        {
            // extract from JAL to preserve patch location
            sceBootLfatOpen = (void*)EXTRACT_J_ADDR(0x8860288C); // 6.61

            // patch io to redirect
            MAKE_CALL(0x8860288C, sceBootLfatOpenPatched); // 6.61
#ifdef MS0_LOAD
            // extract from JAL to preserve patch location
            sceBootLfatRead = (void*)EXTRACT_J_ADDR(0x886028FC);  // 6.61
            sceBootLfatClose = (void*)EXTRACT_J_ADDR(0x88602928); // 6.61

            MAKE_CALL(0x886028FC, sceBootLfatReadPatched);  // 6.61
            MAKE_CALL(0x88602928, sceBootLfatClosePatched); // 6.61

            // prevent lseek errors for our memory based btcnf entries
            _sw(0x00000000, 0x886028D8); // 6.61
            _sw(0x00000000, 0x886028F0); // 6.61
#endif
            break;
        }

        case PSP_MODEL_PSPGO:
        {
            // extract from JAL to preserve patch location
            sceBootLfatOpen = (void*)EXTRACT_J_ADDR(0x8860288C); // 6.61

            // patch io to redirect
            MAKE_CALL(0x8860288C, sceBootLfatOpenPatched); // 6.61

#ifdef MS0_LOAD
            // extract from JAL to preserve patch location
            sceBootLfatRead = (void*)EXTRACT_J_ADDR(0x886028FC);  // 6.61
            sceBootLfatClose = (void*)EXTRACT_J_ADDR(0x88602928); // 6.61

            MAKE_CALL(0x886028FC, sceBootLfatReadPatched);  // 6.61
            MAKE_CALL(0x88602928, sceBootLfatClosePatched); // 6.61

            // prevent lseek errors for our memory based btcnf entries
            _sw(0x00000000, 0x886028D8); // 6.61
            _sw(0x00000000, 0x886028F0); // 6.61
#endif
            break;
        }

        case PSP_MODEL_BRITE:
        case PSP_MODEL_BRITE4G:
        case PSP_MODEL_BRITE7G:
        case PSP_MODEL_BRITE9G:
        case PSP_MODEL_STREET:
        default:
        {
            // extract from JAL to preserve patch location
            sceBootLfatOpen = (void*)EXTRACT_J_ADDR(0x8860288C); // 6.61

            // patch io to redirect
            MAKE_CALL(0x8860288C, sceBootLfatOpenPatched); // 6.61

#ifdef MS0_LOAD
            // extract from JAL to preserve patch location
            sceBootLfatRead = (void*)EXTRACT_J_ADDR(0x886028FC);  // 6.61
            sceBootLfatClose = (void*)EXTRACT_J_ADDR(0x88602928); // 6.61

            MAKE_CALL(0x886028FC, sceBootLfatReadPatched);  // 6.61
            MAKE_CALL(0x88602928, sceBootLfatClosePatched); // 6.61

            // prevent lseek errors for our memory based btcnf entries
            _sw(0x00000000, 0x886028D8); // 6.61
            _sw(0x00000000, 0x886028F0); // 6.61
#endif
            break;
        }
    }
}
