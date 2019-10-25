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

#include "reboot.h"
#include "utility.h"

#include <config.h>
#include <pspmacro.h>

#include <pspinit.h>
#include <psploadcore.h>
#include <pspsysmem_kernel.h>

#include <rebootex_interface.h>

#include <stdio.h>
#include <string.h>

int (*g_sony_reboot)(SceKernelBootParam* reboot_param,
                     SceLoadExecInternalParam* exec_param,
                     int api,
                     int initial_rnd) = (void*)SONY_REBOOT_ADDRESS;
REBOOTEX_ENTRY g_rebootex_entry = (void*)REBOOTEX_ADDRESS;

static char* g_rebootex = (char*)REBOOTEX_ADDRESS;
static int g_rebootex_size = 0;
static u32 g_sony_reboot_backup[2];
static int g_load_compat = 0;
static int g_compat_loaded = 0;
static char* g_compat = (char*)COMPATIBILITY_MODULE_ADDR;
static int g_compat_size = 0;
static int g_firmware_version = 0;
int g_rebootset = 0;

static int onSonyRebootStart(SceKernelBootParam* reboot_param,
                             SceLoadExecInternalParam* exec_param,
                             int api,
                             int initial_rnd)
{
    ClearCaches();
    g_rebootex_entry(
        reboot_param, exec_param, api, initial_rnd, g_compat_loaded, g_firmware_version);

    memcpy((void*)SONY_REBOOT_ADDRESS, g_sony_reboot_backup, 8);
    ClearCaches();
    return g_sony_reboot(reboot_param, exec_param, api, initial_rnd);
}

static int RebootStartPatched(SceKernelBootParam* reboot_param,
                              SceLoadExecInternalParam* exec_param,
                              int api,
                              int initial_rnd)
{
    memcpy((void*)REBOOTEX_ADDRESS, g_rebootex, g_rebootex_size);

    if (g_compat_loaded)
    {
        memcpy((void*)COMPATIBILITY_MODULE_ADDR, g_compat, g_compat_size);
    }

    // need to patch AFTER custom firmware rebootex, if any
    memcpy(g_sony_reboot_backup, (void*)SONY_REBOOT_ADDRESS, 8);
    REDIRECT_FUNCTION(SONY_REBOOT_ADDRESS, onSonyRebootStart);
    ClearCaches();
    return 0;
}

static int readCompatModule(void)
{
    char compat_path[256];
    int res = infInitConfig(INFINITY_CONFIGURATION_LOCATION);

    if (res < 0)
    {
        return 0;
    }

    // read the path to the compatibility module
    res = infGetEntry("inf_boot_compat_path", compat_path, sizeof(compat_path));

    if (res < 0)
    {
        return 0;
    }

    int size = FileSize(compat_path);

    if (size > 0)
    {
        SceUID memid = sceKernelAllocPartitionMemory(1, "reboot_compat", PSP_SMEM_Low, size, NULL);

        if (memid >= 0)
        {
            g_compat = sceKernelGetBlockHeadAddr(memid);
            g_compat_size = ReadFile(compat_path, 0, g_compat, size);
            return 1;
        }
    }

    return 0;
}

static void injectRebootCallback(void)
{
    g_firmware_version = sceKernelDevkitVersion();
    SceModule2* mod = sceKernelFindModuleByName("sceLoadExec");

    // patch reboot in order to reload this module
    switch (sceKernelGetModel())
    {
        case PSP_MODEL_PHAT:
        case PSP_MODEL_SLIM:
        case PSP_MODEL_BRITE:
        case PSP_MODEL_BRITE4G:
        case PSP_MODEL_BRITE7G:
        case PSP_MODEL_BRITE9G:
        case PSP_MODEL_STREET:
        default:
            MAKE_CALL(mod->text_addr + 0x00002DAC - 0x18, RebootStartPatched); // 6.61
            break;

        case PSP_MODEL_PSPGO:
            MAKE_CALL(mod->text_addr + 0x00002FF8 - 0x18, RebootStartPatched); // 6.61
            break;
    }

    ClearCaches();
}

void setLoadCompatModule(int load)
{
    g_load_compat = load;
}

void loadRebootModules(void)
{
    if (g_load_compat)
    {
        g_compat_loaded = readCompatModule();
    }

    int size = FileSize(REBOOTEX_EXEC_PATH);

    if (size > 0)
    {
        SceUID memid = sceKernelAllocPartitionMemory(1, "reboot", PSP_SMEM_Low, size, NULL);

        if (memid >= 0)
        {
            g_rebootset = 1;
            g_rebootex = sceKernelGetBlockHeadAddr(memid);
            g_rebootex_size = ReadFile(REBOOTEX_EXEC_PATH, 0, g_rebootex, size);
            injectRebootCallback();
        }
    }
}

int module_reboot_phase(int reboot_stage, void* reboot_param, u32 unk, u32 unk_2)
{
    if (reboot_stage == REBOOT_STAGE_PRE_KERNEL_KILLED)
    {
        injectRebootCallback();
    }

    return 0;
}
