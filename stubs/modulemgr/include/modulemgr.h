#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <pspmodulemgr.h>

SceUID sceKernelGetModuleIdByAddress(uintptr_t ptr);

#ifdef __cplusplus
}
#endif
