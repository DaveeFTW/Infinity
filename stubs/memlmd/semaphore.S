	.set noreorder

#include "pspstub.s"

	STUB_START	"semaphore",0x00090011,0x00010005
	STUB_FUNC	0x4C537C72,sceUtilsBufferCopyWithRange
	STUB_END
