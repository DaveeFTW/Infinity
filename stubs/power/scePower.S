	.set noreorder

#include "pspstub.s"

STUB_START "scePower", 0x40010000, 0x00020005
STUB_FUNC 0x2B7C7CF4, scePowerRequestStandby
STUB_FUNC 0x0442D852, scePowerRequestColdReset
STUB_END