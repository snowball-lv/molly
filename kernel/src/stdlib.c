#include <stdlib.h>
#include <time.h>
#include <console.h>

void copy(void *src, void *dst, uword num) {
	for (uword i = 0; i < num; i++)
		((byte *)dst)[i] = ((byte *)src)[i];
}

void memset(void *ptr, byte value, uword num) {
	for (uword i = 0; i < num; i++)
		((byte *)ptr)[i] = value;
}