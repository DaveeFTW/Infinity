	.set noreorder

#include "pspstub.s"

	STUB_START "sceSyscon_driver",0x00090000,0x00010005
	STUB_FUNC  0x7EC5A957,sceSysconGetBaryonVersion
	STUB_END
