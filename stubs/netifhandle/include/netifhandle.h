#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SceKernelIfHandleParam
{
	struct SceKernelIfHandleParam *unk_0;
	unsigned int unk_4;
	unsigned int unk_8;
	unsigned int unk_12;
	unsigned short unk_16;
	unsigned short unk_18;
	unsigned int unk_20;
	unsigned int unk_24;
	unsigned int unk_28;
	unsigned int unk_32;
	unsigned int unk_36;
	unsigned int unk_40;
	unsigned int unk_44;
	unsigned int unk_48;
	unsigned int unk_52;
	unsigned int unk_56;
	unsigned int unk_60;
	unsigned int unk_64;
	unsigned int unk_68;
	unsigned int unk_72;
} SceKernelIfHandleParam;

int sceNetMPulldown(SceKernelIfHandleParam *param, int unk, int unk2, void *unk3);

#ifdef __cplusplus
}
#endif
