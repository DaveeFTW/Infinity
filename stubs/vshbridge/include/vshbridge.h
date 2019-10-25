#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <pspctrl.h>
#include <pspmodulemgr.h>

#include <stddef.h>

    int vshKernelExitVSHVSH(void* unk);
    int vshKernelLoadModuleVSH(const char* filename, int flags, const SceKernelLMOption* opt);
    int vshKernelLoadModuleBufferVSH(unsigned int size,
                                     const void* buffer,
                                     int flags,
                                     const SceKernelLMOption* option);
    int vshCtrlReadBufferPositive(SceCtrlData* pad, int samples);

#ifdef __cplusplus
}
#endif
