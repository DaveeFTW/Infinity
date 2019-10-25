	.set noreorder

#include "pspstub.s"

STUB_START "sceVshBridge", 0x40010000, 0x00010005
STUB_FUNC 0xD47041CA, vshKernelExitVSHVSH
STUB_END