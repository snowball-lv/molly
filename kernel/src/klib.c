#include <klib.h>
#include <pit.h>

void halt() {
	__asm__("hlt");
}

void stop() {
	__asm__("cli");
	__asm__("hlt");
}

clock_t clock() {
	return pit_get_ticks();
}

void sleep(uintmax_t millis) {
	clock_t now = clock();
	float secs = (float)millis/1000.f;
	clock_t target = now + secs * CLOCKS_PER_SEC;
	while (clock() < target)
		__asm__("hlt");
}





















