	.set noreorder

#include "pspstub.s"

STUB_START "ModuleMgrForUser", 0x40010000, 0x00010005
STUB_FUNC 0xD8B73127, sceKernelGetModuleIdByAddress
STUB_END