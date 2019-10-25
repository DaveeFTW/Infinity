	.set noreorder

#include "pspstub.s"

	STUB_START	"sceNetIfhandle_lib",0x40090000,0x00010005
	STUB_FUNC	0xE80F00A4, sceNetMPulldown
	STUB_END
