#include <klib.h>
#include <pit.h>
#include <console.h>

void halt() {
	__asm__("hlt");
}

void stop() {
	__asm__("cli");
	__asm__("hlt");
}

void panic(const char *msg) {
	kprintfln(msg);
	stop();
}

void enable_ints() {
	__asm__("sti");
}

void disable_ints() {
	__asm__("cli");
}

const char *kstrchr(const char *str, int character) {
	while (*str != 0 && *str != character)
		str++;
	return str;
}
















