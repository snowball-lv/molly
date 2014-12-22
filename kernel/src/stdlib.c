#include <stdlib.h>

void copy(byte *src, byte *dst, uword num) {
	for (uword i = 0; i < num; i++)
		dst[i] = src[i];
}