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

#include "compatibility.h"
#include "btcnf.h"

#include <compat_interface.h>

int g_is_compat = 0;
int g_psp_model = 0;
int g_is_recovery = 0;
int g_firmware = 0;

COMPAT_ENTRY g_exec_compat_module = (void*)COMPATIBILITY_MODULE_ADDR;

void setCompatibilityInformation(int is_compat, int model, int is_recovery, int firmware)
{
    g_is_compat = is_compat;
    g_psp_model = model;
    g_is_recovery = is_recovery;
    g_firmware = firmware;
}

int compatibilityModuleExists(void)
{
    return g_is_compat;
}

int runCompatibilityModule(BtcnfHeader* btcnf, int size)
{
    return g_exec_compat_module(btcnf,
                                size,
                                InsertModuleBtcnfBefore,
                                ReplaceModuleBtcnf,
                                g_psp_model,
                                g_is_recovery,
                                g_firmware);
}
