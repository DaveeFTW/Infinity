#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

int sceUtilsBufferCopyWithRange(void *dst, size_t dst_size, const void *src, size_t src_size, int cmd);

#ifdef __cplusplus
}
#endif
