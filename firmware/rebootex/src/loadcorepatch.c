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

#include "loadcorepatch.h"
#include "utility.h"

#include <psploadcore.h>
#include <pspmacro.h>

#include <string.h>

#define BOOT_MODULE_ID_SYSMEM (0)
#define BOOT_MODULE_ID_LOADCORE (1)

int (*sceKernelRegisterModule)(SceModule2* mod) = NULL;
int (*loadPrx)(u32 a0, void* info, u32 module_id, u32 a3) = NULL;
int (*CheckLatestSubType)(PSP_Header* header) = NULL;

// loadcore patch functions
void InjectLoadCorePatch(void);
void InjectLoadCorePatch01g(void);
void RestoreExecuteLoadCore(SceSize args, void* argp);

u32 loadcore_addr = 0;
u32 g_loadcore_bootstart = 0;
u32 g_loadcore_bootstart_backup[2];

static int sceKernelRegisterModulePatched(SceModule2* mod)
{
    if (memcmp(mod->modname, "sceMemlmd", strlen("sceMemlmd") + 1) == 0)
    {
        // force whitelist check to return 1
        MAKE_FUNCTION_RETURN(mod->text_addr + 0xA8, 1); // 6.60/6.61

        // remove this hook. we no longer need it
        MAKE_CALL(loadcore_addr + 0x18BC, (uintptr_t)sceKernelRegisterModule);
        ClearCaches();
    }

    return sceKernelRegisterModule(mod);
}

static int CheckLatestSubTypePatched(PSP_Header* header)
{
    if (header->tag == 0)
    {
        MAKE_CALL(loadcore_addr + 0x58F8, (uintptr_t)CheckLatestSubType);
        ClearCaches();
        return 1;
    }

    return CheckLatestSubType(header);
}

static int loadPrxPatched(u32 a0, void* info, u32 module_id, u32 a3)
{
    int res = loadPrx(a0, info, module_id, a3);

    if (module_id == BOOT_MODULE_ID_LOADCORE)
    {
        uintptr_t bootstart = *(uintptr_t*)((uintptr_t)info + 0x28);
        uintptr_t text_addr = bootstart - 0xAF8;
        loadcore_addr = text_addr;

        // hook module registration so we can patch memlmd
        sceKernelRegisterModule = (void*)EXTRACT_J_ADDR(text_addr + 0x18BC);
        MAKE_CALL(text_addr + 0x18BC, sceKernelRegisterModulePatched);

        // hook CheckLatestSubType to allow 0 tag modules (our kprx)
        CheckLatestSubType = (void*)EXTRACT_J_ADDR(text_addr + 0x58F8);
        MAKE_CALL(text_addr + 0x58F8, CheckLatestSubTypePatched);

        ClearCaches();
    }

    return res;
}

void applyLoadcorePatch(int model)
{
    switch (model)
    {
        case PSP_MODEL_PHAT:
        {
            // hook loadcore decryption so we can apply some patches
            loadPrx = (void*)EXTRACT_J_ADDR(0x886055B0); // 6.60/6.61
            MAKE_CALL(0x886055B0, loadPrxPatched);       // 6.60/6.61
            break;
        }

        case PSP_MODEL_SLIM:
        {
            // hook loadcore decryption so we can apply some patches
            loadPrx = (void*)EXTRACT_J_ADDR(0x88605670); // 6.60/6.61
            MAKE_CALL(0x88605670, loadPrxPatched);       // 6.60/6.61
            break;
        }

        case PSP_MODEL_PSPGO:
        {
            // hook loadcore decryption so we can apply some patches
            loadPrx = (void*)EXTRACT_J_ADDR(0x88605670); // 6.60/6.61
            MAKE_CALL(0x88605670, loadPrxPatched);       // 6.60/6.61
            break;
        }

        case PSP_MODEL_BRITE:
        case PSP_MODEL_BRITE4G:
        case PSP_MODEL_BRITE7G:
        case PSP_MODEL_BRITE9G:
        case PSP_MODEL_STREET:
        default:
        {
            // hook loadcore decryption so we can apply some patches
            loadPrx = (void*)EXTRACT_J_ADDR(0x88605670); // 6.60/6.61
            MAKE_CALL(0x88605670, loadPrxPatched);       // 6.60/6.61
            break;
        }
    }
}
