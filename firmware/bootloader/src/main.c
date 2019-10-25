#include <kexploit.h>
#include <pspmacro.h>
#include <vshbridge.h>

#include <pspctrl.h>
#include <pspkernel.h>
#include <pspmodulemgr.h>
#include <pspsdk.h>

#include <modulemgr.h>

PSP_MODULE_INFO("infinity_bootloader", 0, 1, 0);

int module_start(SceSize args, void* argp);

int g_do_logging = 0;

enum ErrorCodes
{
    KernelModeError = 0,
    CreateThreadError,
    StartThreadError,
    OpenModuleError,
    AllocModuleMemoryError,
    ReadModuleError,
    LoadModuleError,
    StartModuleError,
    WarmResetError
};

void* memset(void* ptr, int value, size_t num)
{
    char* p = (char*)ptr;
    char* end = p + num;

    while (p != end)
    {
        *p++ = value;
    }

    return ptr;
}

int memcmp(const void* ptr1, const void* ptr2, size_t n)
{
    const char* p1 = (const char*)ptr1;
    const char* p2 = (const char*)ptr2;

    for (size_t i = 0; i < n; ++i)
    {
        int v = p1[i] - p2[i];

        if (v)
        {
            return v;
        }
    }

    return 0;
}

int strcmp(const char* str1, const char* str2)
{
    while (*str1 == *str2 && *str1 && *str2)
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

static void disableKernelModuleWhitelist(const KernelContext* ctx)
{
    uintptr_t baseAddress = ctx->pspKernelFindModuleByName("sceMemlmd")->text_addr;

    // force whitelist check to return 1
    MAKE_FUNCTION_RETURN(baseAddress + 0xA8, 1); // 6.60/6.61
    ctx->clearCaches();
}

static void emergencyRecovery(void)
{
    uintptr_t loadAddress = 0x09000000;

    // last ditch attempt to have some external influence
    SceUID fd = sceIoOpen("ms0:/recovery.bin", PSP_O_RDONLY, 0777);

    if (fd >= 0)
    {
        sceIoRead(fd, &loadAddress, sizeof(loadAddress));
        sceIoRead(fd, (void*)loadAddress, 1 * 1024 * 1024);
        sceIoClose(fd);

        sceKernelDcacheWritebackInvalidateAll();
        sceKernelIcacheInvalidateAll();

        void (*recovery)(uintptr_t ptr) = (void*)loadAddress;
        recovery((uintptr_t)module_start);
    }
}

static void appendErrorCode(int code, int error)
{
    SceUID fd =
        sceIoOpen("ms0:/infinity_error.bin", PSP_O_WRONLY | PSP_O_APPEND | PSP_O_CREAT, 0777);

    if (fd >= 0)
    {
        sceIoWrite(fd, &code, 4);
        sceIoWrite(fd, &error, 4);
        sceIoClose(fd);
    }
}

int kloader_thread(SceSize args, void* argp)
{
    SceUID modmgr_thid = *(SceUID*)argp;
    sceKernelWaitThreadEnd(modmgr_thid, NULL);
    sceKernelDelayThread(5000);

    // first step is to escalate privileges to kernel mode and disable the module whitelist
    // from there we can load a native kernel module
    int res = executeInKernelMode(disableKernelModuleWhitelist);

    if (g_do_logging && res < 0)
    {
        appendErrorCode(KernelModeError, res);
        return 0;
    }

    SceUID fd = sceIoOpen(INFINITYBOOT_EXEC_PATH, PSP_O_RDONLY, 0777);

    if (g_do_logging && fd < 0)
    {
        appendErrorCode(OpenModuleError, fd);
        return 0;
    }

    int size = sceIoLseek32(fd, 0, PSP_SEEK_END);
    sceIoLseek32(fd, 0, PSP_SEEK_SET);

    SceUID memid = sceKernelAllocPartitionMemory(2, "infinityrebooter", PSP_SMEM_Low, size, NULL);

    if (g_do_logging && memid < 0)
    {
        appendErrorCode(AllocModuleMemoryError, memid);
        return 0;
    }

    void* moduleData = sceKernelGetBlockHeadAddr(memid);

    res = sceIoRead(fd, moduleData, size);

    if (g_do_logging && res != size)
    {
        appendErrorCode(ReadModuleError, res);
        return 0;
    }

    sceIoClose(fd);

    SceKernelLMOption opt;
    memset(&opt, 0, sizeof(opt));

    opt.size = sizeof(opt);
    opt.mpidtext = 1;
    opt.mpiddata = 1;
    opt.position = 0;
    opt.access = 1;

    // we now want to load our infinity rebooter
    SceUID modid = vshKernelLoadModuleBufferVSH(size, moduleData, 0, &opt);

    if (g_do_logging && modid < 0)
    {
        appendErrorCode(LoadModuleError, modid);
        return 0;
    }

    res = sceKernelStartModule(modid, 0, NULL, NULL, NULL);

    if (g_do_logging && res < 0)
    {
        appendErrorCode(StartModuleError, modid);
        return 0;
    }

    // we can't reliably setup CFW in this boot. Best to warm reset
    res = vshKernelExitVSHVSH(NULL);

    if (g_do_logging && res < 0)
    {
        appendErrorCode(WarmResetError, res);
        return 0;
    }

    return 0;
}

int interceptMemset(void* dst, int v, size_t size, uintptr_t ra)
{
    if (size != 0x100)
    {
        return 0;
    }

    SceUID modid = sceKernelGetModuleIdByAddress(ra);

    if (modid < 0)
    {
        return 0;
    }

    SceKernelModuleInfo info = { 0 };
    info.size = sizeof(info);
    int res = sceKernelQueryModuleInfo(modid, &info);

    // interestingly, vsh threads still do not have permission to query
    // vsh module information. Since vshmain is the only module with vsh permissions,
    // we can identify it by checking for an error from sceKernelQueryModuleInfo.
    if (res == SCE_KERNEL_ERROR_CANNOT_GET_MODULE_INFORMATION)
    {
        // create a new thread to work from and inherit the VSH attributes
        SceUID thid = sceKernelCreateThread(
            "kloader", kloader_thread, 0x18, 0x1000, PSP_THREAD_ATTR_VSH, NULL);

        if (g_do_logging && thid < 0)
        {
            appendErrorCode(CreateThreadError, thid);
            return 0;
        }

        SceUID modmgr_thid = sceKernelGetThreadId();
        res = sceKernelStartThread(thid, sizeof(modmgr_thid), &modmgr_thid);

        if (g_do_logging && res < 0)
        {
            appendErrorCode(StartThreadError, res);
            return 0;
        }

        // force this thread (SceModMgrStart) to exit. We don't want vshmain to try and run.
        while (1)
        {
            sceKernelExitThread(0);
        }
    }

    return 0;
}

int kernelLibraryHandlerStub(void)
{
    return 0;
}

int module_start(SceSize args, void* argp)
{
    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);

    // check for the emergency recovery combo
    if ((pad.Buttons & (PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE)) ==
        (PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE))
    {
        emergencyRecovery();
    }

    // check for the logging combo
    g_do_logging = ((pad.Buttons & (PSP_CTRL_RTRIGGER | PSP_CTRL_SQUARE)) ==
                    (PSP_CTRL_RTRIGGER | PSP_CTRL_SQUARE));

    return 0;
}
