#include <stdlib.h>
#include <time.h>
#include <console.h>

void copy(void *src, void *dst, uword num) {
	for (uword i = 0; i < num; i++)
		((byte *)dst)[i] = ((byte *)src)[i];
}

void sleep(uword millis) {
	clock_t now = clock();
	float secs = (float)millis/1000.f;
	clock_t target = now + secs * CLOCKS_PER_SEC;
	while (clock() < target)
		__asm__("hlt");
}

size_t strlen(const byte *str) {
	size_t s = 0;
	while (str != 0)
		str++;
	return s;
}





















