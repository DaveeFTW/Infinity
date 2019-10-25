#include <pspimpose_driver.h>
#include <pspkernel.h>
#include <pspmodulemgr_kernel.h>
#include <pspsdk.h>
#include <pspsysmem_kernel.h>

#include <kernel_helper.h>
#include <memlmd.h>
#include <string.h>
#include <syscon.h>

#include <pspmacro.h>

PSP_MODULE_INFO("kernel_helper", 0x1000, 1, 0);

int bss = 0;

void ClearCaches(void)
{
    // Clear the Icache
    asm("\
	.word 0x40088000; .word 0x24091000; .word 0x7D081240;\
	.word 0x01094804; .word 0x4080E000; .word 0x4080E800;\
	.word 0x00004021; .word 0xBD010000; .word 0xBD030000;\
	.word 0x25080040; .word 0x1509FFFC; .word 0x00000000;\
	" ::);

    // Clear the dcache
    asm("\
	.word 0x40088000; .word 0x24090800; .word 0x7D081180;\
	.word 0x01094804; .word 0x00004021; .word 0xBD140000;\
	.word 0xBD140000; .word 0x25080040; .word 0x1509FFFC;\
	.word 0x00000000; .word 0x0000000F; .word 0x00000000;\
	" ::);
}

void boot_infinity(void)
{
    u32 k1 = pspSdkSetK1(0);
    ClearCaches();
    pspSdkSetK1(k1);
}
int getTrueModel(void)
{
    u32 k1 = pspSdkSetK1(0);
    ClearCaches();
    pspSdkSetK1(k1);
    return 0;
}
int setHomePopup(int value)
{
    u32 k1 = pspSdkSetK1(0);
    ClearCaches();
    pspSdkSetK1(k1);
    return 0;
}

////////// SignCheck //////////

u8 check_keys0[0x10] = { 0x71, 0xF6, 0xA8, 0x31, 0x1E, 0xE0, 0xFF, 0x1E,
                         0x50, 0xBA, 0x6C, 0xD2, 0x98, 0x2D, 0xD6, 0x2D };

u8 check_keys1[0x10] = { 0xAA, 0x85, 0x4D, 0xB0, 0xFF, 0xCA, 0x47, 0xEB,
                         0x38, 0x7F, 0xD7, 0xE4, 0x3D, 0x62, 0xB0, 0x10 };

static int Encrypt(u32* buf, int size)
{
    buf[0] = 4;
    buf[1] = buf[2] = 0;
    buf[3] = 0x100;
    buf[4] = size;

    /* Note: this encryption returns different data in each psp,
       But it always returns the same in a specific psp (even if it has two nands) */
    if (sceUtilsBufferCopyWithRange(buf, size + 0x14, buf, size + 0x14, 5) != 0)
        return -1;

    return 0;
}

int pspSignCheck(u8* buf)
{
    u8 encbuf[0xD0 + 0x14 + 0x40];
    int iXOR, res;
    int k1 = pspSdkSetK1(0);
    u8* enc = encbuf;

    while ((uintptr_t)enc % 64)
    {
        enc++;
    }

    memcpy(enc + 0x14, buf + 0x110, 0x40);
    memcpy(enc + 0x14 + 0x40, buf + 0x80, 0x90);

    for (iXOR = 0; iXOR < 0xD0; iXOR++)
    {
        enc[0x14 + iXOR] ^= check_keys0[iXOR & 0xF];
    }

    if ((res = Encrypt((u32*)enc, 0xD0)) != 0)
    {
        pspSdkSetK1(k1);
        return -1;
    }

    for (iXOR = 0; iXOR < 0xD0; iXOR++)
    {
        enc[0x14 + iXOR] ^= check_keys1[iXOR & 0xF];
    }

    memcpy(buf + 0x80, enc + 0x14, 0xD0);

    pspSdkSetK1(k1);
    return 0;
}

int module_start(SceSize args, void* argp)
{
    return 0;
}
