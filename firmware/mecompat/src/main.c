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

#include <pspmacro.h>

#include <pspinit.h>
#include <psploadcore.h>
#include <pspsdk.h>

#include <compat_interface.h>

#include <string.h>

#define REBOOT_START (0x88600000)

// loadcore signcheck decryption functions
int (*DecryptExecutable)(void* buffer, int size, int* outsize) = NULL;
int (*UnSigncheck)(void* buffer, int size) = NULL;

void* memcpy(void* dst, const void* src, size_t len)
{
    void* pRet = dst;
    const char* usrc = (const char*)src;
    char* udst = (char*)dst;

    while (len > 0)
    {
        *udst++ = *usrc++;
        len--;
    }

    return pRet;
}

int memcmp(const void* b1, const void* b2, size_t len)
{
    int val = 0;
    const unsigned char *u1, *u2;

    u1 = (const unsigned char*)b1;
    u2 = (const unsigned char*)b2;

    while (len > 0)
    {
        if (*u1 != *u2)
        {
            val = (int)*u1 - (int)*u2;
            break;
        }
        u1++;
        u2++;
        len--;
    }

    return val;
}

void* memmove(void* dst, const void* src, size_t len)
{
    void* pRet = dst;
    char* udst;
    const char* usrc;

    if (dst < src)
    {
        /* Copy forwards */
        udst = (char*)dst;
        usrc = (const char*)src;
        while (len > 0)
        {
            *udst++ = *usrc++;
            len--;
        }
    }
    else
    {
        /* Copy backwards */
        udst = ((char*)dst) + len;
        usrc = ((const char*)src) + len;
        while (len > 0)
        {
            *--udst = *--usrc;
            len--;
        }
    }

    return pRet;
}

void* memset(void* b, int c, size_t len)
{
    void* pRet = b;
    unsigned char* ub = (unsigned char*)b;

    while (len > 0)
    {
        *ub++ = (unsigned char)c;
        len--;
    }

    return pRet;
}

void xor_cipher(u8* data, u32 size, u8* key, u32 key_size)
{
    u32 i;

    for (i = 0; i < size; i++)
    {
        data[i] ^= key[i % key_size];
    }
}

int DecryptExecutablePatched(PSP_Header* header, int size, int* outsize)
{
    /* check for decryption tag */
    if (header->oe_tag == 0xC6BA41D3)
    {
        xor_cipher((u8*)header + sizeof(PSP_Header), 0x10, header->key_data1, 0x10);
        xor_cipher(
            (u8*)header + sizeof(PSP_Header), header->comp_size, &header->scheck[0x38], 0x20);
        *outsize = header->comp_size;
        memmove(header, (u8*)header + sizeof(PSP_Header), header->comp_size);
        return 0;
    }

    /* return the real decryption code */
    return DecryptExecutable(header, size, outsize);
}

int UnSigncheckPatched(void* buffer, int size)
{
    int i;
    /* loop through the signcheck */
    for (i = 0; i < 0x30; i++)
    {
        /* if byte is 0 then call the signcheck removal */
        if (((u8*)buffer)[0xD4 + i])
        {
            /* remove signcheck */
            return UnSigncheck(buffer, size);
        }
    }

    return 0;
}
int PatchLoadCore(void* a0, void* a1, void* a2, int (*module_start)(void*, void*, void*))
{
    u32 text_addr = ((u32)module_start) - 0x00000AF8;

    MAKE_CALL(text_addr + 0x00005994, UnSigncheckPatched);
    MAKE_CALL(text_addr + 0x00005970, DecryptExecutablePatched);

    UnSigncheck = (void*)(text_addr + 0x00007824);
    DecryptExecutable = (void*)(text_addr + 0x0000783C);

    return module_start(a0, a1, a2);
}

const char* get_config(int model)
{
    switch (model)
    {
        case PSP_MODEL_PHAT:
            return "/kd/pspbtjnf.bin";
        case PSP_MODEL_SLIM:
            return "/kd/pspbtjnf_02g.bin";
        case PSP_MODEL_BRITE:
            return "/kd/pspbtjnf_03g.bin";
        case PSP_MODEL_BRITE4G:
            return "/kd/pspbtjnf_04g.bin";
        case PSP_MODEL_BRITE7G:
            return "/kd/pspbtjnf_07g.bin";
        case PSP_MODEL_BRITE9G:
            return "/kd/pspbtjnf_09g.bin";
        case PSP_MODEL_STREET:
            return "/kd/pspbtjnf_11g.bin";
        case PSP_MODEL_PSPGO:
            return "/kd/pspbtjnf_05g.bin";
    }

    return "/kd/pspbtcnf.bin";
}

int compat_entry(BtcnfHeader* btcnf,
                 int btcnf_size,
                 INSERT_BTCNF insert_btcnf,
                 REPLACE_BTCNF replace_btcnf,
                 int model,
                 int is_recovery,
                 int firmware)
{
    int (*sceBootLfatOpen)(const char* path) = NULL;
    int (*sceBootLfatRead)(void* data, int size) = NULL;
    int (*sceBootLfatClose)(void) = NULL;

    // clear config
    memset((void*)0x88FB0000, 0, 0x100);

    switch (model)
    {
        case PSP_MODEL_PHAT:
        {
            // extract from JAL to preserve patch location
            sceBootLfatOpen = (void*)EXTRACT_J_ADDR(0x886027C4);  // 6.61
            sceBootLfatRead = (void*)EXTRACT_J_ADDR(0x88602834);  // 6.61
            sceBootLfatClose = (void*)EXTRACT_J_ADDR(0x88602860); // 6.61

            break;
        }

        case PSP_MODEL_SLIM:
        case PSP_MODEL_BRITE:
        case PSP_MODEL_BRITE4G:
        case PSP_MODEL_BRITE7G:
        case PSP_MODEL_BRITE9G:
        case PSP_MODEL_STREET:
        case PSP_MODEL_PSPGO:
        default:
        {
            // extract from JAL to preserve patch location
            sceBootLfatOpen = (void*)EXTRACT_J_ADDR(0x8860288C);  // 6.61
            sceBootLfatRead = (void*)EXTRACT_J_ADDR(0x886028FC);  // 6.61
            sceBootLfatClose = (void*)EXTRACT_J_ADDR(0x88602928); // 6.61

            break;
        }
    }

    if (sceBootLfatOpen(get_config(model)) < 0)
        return btcnf_size;

    int new_size = sceBootLfatRead(btcnf, 1 * 1024 * 1024);
    sceBootLfatClose();

    switch (model)
    {
        case PSP_MODEL_PHAT:
            // Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as
            // fourth argument.
            _sw(0x00113821, REBOOT_START + 0x00005644 - 4);
            // Prepare LoadCore Patch Part #2 - jal PatchLoadCore
            MAKE_JUMP(REBOOT_START + 0x00005644, PatchLoadCore);
            // Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
            _sw(0x02A0E821, REBOOT_START + 0x00005644 + 4);
            break;

        case PSP_MODEL_SLIM:
        case PSP_MODEL_BRITE:
        case PSP_MODEL_BRITE4G:
        case PSP_MODEL_BRITE7G:
        case PSP_MODEL_BRITE9G:
        case PSP_MODEL_STREET:
        case PSP_MODEL_PSPGO:
        default:
            // Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as
            // fourth argument.
            _sw(0x00113821, REBOOT_START + 0x00005704 - 4);
            // Prepare LoadCore Patch Part #2 - jal PatchLoadCore
            MAKE_JUMP(REBOOT_START + 0x00005704, PatchLoadCore);
            // Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
            _sw(0x02A0E821, REBOOT_START + 0x00005704 + 4);
            break;
    }

    if (is_recovery)
    {
        replace_btcnf("/kd/usbstorms.prx", "/kd/usersystemlib.prx", btcnf, &new_size, BOOTLOAD_VSH);
        replace_btcnf(
            "/kd/usbstorboot.prx", "/kd/libatrac3plus.prx", btcnf, &new_size, BOOTLOAD_VSH);
        replace_btcnf("/kd/usbstor.prx", "/kd/mediasync.prx", btcnf, &new_size, BOOTLOAD_VSH);
        replace_btcnf("/kd/usbstormgr.prx", "/kd/vshctrl_02g.prx", btcnf, &new_size, BOOTLOAD_VSH);
        replace_btcnf("/kd/usbdev.prx", "/vsh/module/paf.prx", btcnf, &new_size, BOOTLOAD_VSH);
        replace_btcnf(
            "/kd/lflash_fatfmt.prx", "/vsh/module/common_gui.prx", btcnf, &new_size, BOOTLOAD_VSH);
        replace_btcnf(
            "/kd/usersystemlib.prx", "/vsh/module/common_util.prx", btcnf, &new_size, BOOTLOAD_VSH);
        replace_btcnf(
            "/vsh/module/recovery.prx", "/vsh/module/vshmain.prx", btcnf, &new_size, BOOTLOAD_VSH);

        if (model == PSP_MODEL_PSPGO)
        {
            replace_btcnf(
                "/kd/usbstoreflash.prx", "/module/mcore.prx", btcnf, &new_size, BOOTLOAD_VSH);
        }
    }

    // return the new btcnf size
    return new_size;
}
