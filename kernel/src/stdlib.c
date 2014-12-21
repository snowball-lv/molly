#include <stdlib.h>

void copy(byte *src, byte *dst, uword count) {
	for (uword i = 0; i < count; i++)
		dst[i] = src[i];
}