#include <stdlib.h>
#include <time.h>
#include <console.h>

void copy(void *src, void *dst, uword num) {
	for (uword i = 0; i < num; i++)
		((byte *)dst)[i] = ((byte *)src)[i];
}

void memset(void *ptr, byte value, size_t num) {
	for (size_t i = 0; i < num; i++)
		((byte *)ptr)[i] = value;
}

void sleep(uword millis) {
	clock_t now = clock();
	float secs = (float)millis/1000.f;
	clock_t target = now + secs * CLOCKS_PER_SEC;
	while (clock() < target)
		__asm__("hlt");
}

void halt() {
	__asm__("cli");
	__asm__("hlt");
}


















