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

#include "libinfinity.h"

#include <pspmacro.h>
#include <reboot.h>

#include <pspctrl.h>
#include <pspkernel.h>
#include <psploadcore.h>
#include <pspsysmem_kernel.h>

#include <string.h>

PSP_MODULE_INFO("InfinityControl", 0x7007, 1, 0);

int (*PrologueModule)(void* modmgr_param, SceModule2* mod) = (void*)NULL;

int PrologueModulePatched(void* modmgr_param, SceModule2* mod)
{
    u32 text_addr = mod->text_addr;
    int res = PrologueModule(modmgr_param, mod);

    // If this function errors, the module is shutdown so we better check for it
    if (res >= 0)
    {
        if (strcmp(mod->modname, "sceMesgLed") == 0)
        {
            loadRebootModules();
        }

        else if (strcmp(mod->modname, "sysconf_plugin_module") == 0)
        {
            void infinityVersionAppend(void);
            void infinityVersionAppendEnd(void);
            void return_address(void);

            size_t size = (uintptr_t)infinityVersionAppendEnd - (uintptr_t)infinityVersionAppend;
            SceUID block_id = sceKernelAllocPartitionMemory(2, "", PSP_SMEM_Low, size, NULL);

            if (block_id >= 0)
            {
                size_t return_address_offset =
                    (uintptr_t)return_address - (uintptr_t)infinityVersionAppend;
                void* addr = sceKernelGetBlockHeadAddr(block_id);
                u32 func_addr = EXTRACT_J_ADDR(text_addr + 0x00019314) & ~0x80000000; // 6.61

                memcpy(addr, infinityVersionAppend, size);
                _sw(func_addr, (u32)addr + return_address_offset);

                MAKE_CALL(text_addr + 0x00019314, addr);
                sceKernelIcacheInvalidateAll();
                sceKernelDcacheWritebackInvalidateAll();
            }
        }
    }

    return res;
}

void PatchModuleManager(void)
{
    SceModule2* mod = sceKernelFindModuleByName("sceModuleManager");
    u32 text_addr = mod->text_addr;

    // patch call to PrologueModule from the StartModule function to allow a full coverage of loaded
    // modules (even those without an entry point)
    KERNEL_HIJACK_FUNCTION(text_addr + 0x00008124, PrologueModulePatched, PrologueModule); // 6.61
}

int module_start(SceSize args, void* argp)
{
    PatchModuleManager();
    sceKernelIcacheInvalidateAll();
    sceKernelDcacheWritebackInvalidateAll();
    return 0;
}
