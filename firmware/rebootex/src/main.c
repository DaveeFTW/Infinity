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

#include "btcnfpatch.h"
#include "compatibility.h"
#include "lfatpatch.h"
#include "loadcorepatch.h"
#include "syscon.h"
#include "utility.h"

#include <rebootex_interface.h>

void we_got_this_far()
{
    while (1)
    {
        *(volatile unsigned int*)(0xbe24000c) = 0xC0;
        Syscon_wait(250000 / 4);

        *(volatile unsigned int*)(0xbe240008) = 0xC0;
        Syscon_wait(250000 / 4);
    }
}

#ifdef MS0_LOAD
int rebootex_entry(SceKernelBootParam* reboot_param,
                   SceLoadExecInternalParam* exec_param,
                   int api,
                   int initial_rnd,
                   int is_compat,
                   const char* infinityctrl,
                   int size)
{
    setInfinityctrl(infinityctrl, size);
#else
int rebootex_entry(SceKernelBootParam* reboot_param,
                   SceLoadExecInternalParam* exec_param,
                   int api,
                   int initial_rnd,
                   int is_compat,
                   int firmware)
{
#endif
    // taken from IPL SDK
    u32 ctrl = 0, is_recovery = 0;
    _pspSysconGetCtrl1(&ctrl);

    // positive
    ctrl = ~ctrl;

    if ((ctrl & SYSCON_CTRL_RTRG) == SYSCON_CTRL_RTRG)
    {
        is_recovery = 1;
    }

    // if home button is pressed, we want to boot into OFW
    if ((ctrl & SYSCON_CTRL_HOME) == SYSCON_CTRL_HOME)
    {
        setCompatibilityInformation(0, 0, 0, 0);
    }
    else
    {
        setCompatibilityInformation(is_compat, reboot_param->psp_model, is_recovery, firmware);
    }

    // apply all needed patches
    applyBtcnfPatches(reboot_param->psp_model, firmware);
    applyLoadcorePatch(reboot_param->psp_model);

    ClearCaches();

    // just return, let our caller decide on the next action
    return 0;
}
