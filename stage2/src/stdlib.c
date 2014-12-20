#include <stdlib.h>


u32 strcmp(s8 *a, s8 *b, u32 len) {
	for (u32 i = 0; i < len; i++) {
		if (a[i] != b[i])
			return 0;
	}	
	return 1;
}


void copy(u8 *src, u8 *dst, u32 len) {
	for (u32 i = 0; i < len; i++)
		dst[i] = src[i];
}