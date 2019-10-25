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
#include "btcnf.h"
#include "compatibility.h"

#include <pspmacro.h>

// btcnf decryption functions
u32 (*sceRebootLoadModuleBtcnf)(void* btcnf_data,
                                u32 size,
                                void* a2,
                                void* a3,
                                void* t0,
                                void* t1,
                                void* t2) = NULL;

u32* g_lfat_read_buffer = 0;

u32 sceRebootLoadModuleBtcnfPatched(void* btcnf_data,
                                    u32 size,
                                    void* a2,
                                    void* a3,
                                    void* t0,
                                    void* t1,
                                    void* t2)
{
    /* cast our variables and declare */
    BtcnfHeader* header = (BtcnfHeader*)btcnf_data;

    /* check the signature is valid */
    if (header->signature == 0x0F803001)
    {
        int new_btcnfsize = size;

        // run the compatibility module first if it exists
        if (compatibilityModuleExists())
        {
            new_btcnfsize = runCompatibilityModule(btcnf_data, size);
        }

        /* insert the module path into the config */
        InsertModuleBtcnfAfter("/kd/infinityctrl.prx",
                               "/kd/modulemgr.prx",
                               btcnf_data,
                               &new_btcnfsize,
                               (BOOTLOAD_VSH | BOOTLOAD_GAME | BOOTLOAD_POPS | BOOTLOAD_UPDATER |
                                BOOTLOAD_UMDEMU | BOOTLOAD_APP | BOOTLOAD_MLNAPP));
        ReplaceModuleBtcnf("/kd/usersystemlib.inf",
                           "/kd/usersystemlib.prx",
                           btcnf_data,
                           &new_btcnfsize,
                           (BOOTLOAD_VSH | BOOTLOAD_GAME | BOOTLOAD_POPS | BOOTLOAD_UPDATER |
                            BOOTLOAD_UMDEMU | BOOTLOAD_APP | BOOTLOAD_MLNAPP));

        // update buffer by size with 64 alignment
        *g_lfat_read_buffer += ((new_btcnfsize - size) + 64) & ~0x3F;
    }

    /* return the new size */
    return sceRebootLoadModuleBtcnf(btcnf_data, size, a2, a3, t0, t1, t2);
}

void applyBtcnfPatches(int model, int firmware)
{
    switch (model)
    {
        case PSP_MODEL_PHAT:
        {
            // extract from JAL to preserve patch location
            sceRebootLoadModuleBtcnf = (void*)EXTRACT_J_ADDR(0x88607134); // 6.61

            // patch pspbtcnf decryption so we can insert our own module
            MAKE_CALL(0x88607134, sceRebootLoadModuleBtcnfPatched); // 6.61

            // patch btcnf hash check
            _sw(0x00000000, 0x88607390); // 6.61

            // set lfat read buffer pointer so we can extend it when adding new modules
            if (firmware == 0x06060010)
            {
                g_lfat_read_buffer = (u32*)0x88612908; // 6.60
            }
            else if (firmware == 0x06060110)
            {
                g_lfat_read_buffer = (u32*)0x88612BA0; // 6.61
            }

            break;
        }

        case PSP_MODEL_SLIM:
        {
            // extract from JAL to preserve patch location
            sceRebootLoadModuleBtcnf = (void*)EXTRACT_J_ADDR(0x886071F4); // 6.61

            // patch pspbtcnf loading so we can insert our own module
            MAKE_CALL(0x886071F4, sceRebootLoadModuleBtcnfPatched); // 6.61

            // patch btcnf hash check
            _sw(0x00000000, 0x88607450); // 6.61

            // set lfat read buffer pointer so we can extend it when adding new modules
            if (firmware == 0x06060010)
            {
                g_lfat_read_buffer = (u32*)0x886129E8; // 6.60
            }
            else if (firmware == 0x06060110)
            {
                g_lfat_read_buffer = (u32*)0x88612C80; // 6.61
            }

            break;
        }

        case PSP_MODEL_PSPGO:
        {
            // extract from JAL to preserve patch location
            sceRebootLoadModuleBtcnf = (void*)EXTRACT_J_ADDR(0x886071F4); // 6.61

            // patch pspbtcnf loading so we can insert our own module
            MAKE_CALL(0x886071F4, sceRebootLoadModuleBtcnfPatched); // 6.61

            // patch btcnf hash check
            _sw(0x00000000, 0x88607450); // 6.61

            // set lfat read buffer pointer so we can extend it when adding new modules
            if (firmware == 0x06060010)
            {
                g_lfat_read_buffer = (u32*)0x886129E8; // 6.60
            }
            else if (firmware == 0x06060110)
            {
                g_lfat_read_buffer = (u32*)0x88612C80; // 6.61
            }

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
            sceRebootLoadModuleBtcnf = (void*)EXTRACT_J_ADDR(0x886071F4); // 6.61

            // patch pspbtcnf loading so we can insert our own module
            MAKE_CALL(0x886071F4, sceRebootLoadModuleBtcnfPatched); // 6.61

            // patch btcnf hash check
            _sw(0x00000000, 0x88607450); // 6.61

            // set lfat read buffer pointer so we can extend it when adding new modules
            if (firmware == 0x06060010)
            {
                g_lfat_read_buffer = (u32*)0x886129E8; // 6.60
            }
            else if (firmware == 0x06060110)
            {
                g_lfat_read_buffer = (u32*)0x88612C80; // 6.61
            }
        }
    }
}
