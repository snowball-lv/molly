#include <klib.h>

void halt() {
	__asm__("hlt");
}

void stop() {
	__asm__("cli");
	__asm__("hlt");
}

void memset(void *ptr, byte value, size_t num) {
	for (size_t i = 0; i < num; i++)
		((byte *)ptr)[i] = value;
}