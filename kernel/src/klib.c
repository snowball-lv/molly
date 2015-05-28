#include <klib.h>
#include <pit.h>
#include <console.h>
#include <kalloc.h>
#include <string.h>

void halt() {
	__asm__("hlt");
}

void stop() {
	__asm__("cli");
	__asm__("hlt");
}

void enable_ints() {
	__asm__("sti");
}

void disable_ints() {
	__asm__("cli");
}

int enter_critical_section() {
	int state = read_flags() & F_INTERRUPTS;
	__asm__("cli");
	return state;
}

void exit_critical_section(int interrupt_state) {
	if (interrupt_state) //if were enabled
		__asm__("sti");
}

const char *kstrchr(const char *str, int character) {
	while (*str != 0 && *str != character)
		str++;
	return str;
}

char *kstrdup(char *str) {
	size_t len = strlen(str);
	char *dup = kmalloc(len + 1);
	strcpy(dup, str);
	return dup;
}

int strdiff(const char *str1, const char *str2) {

	int i = 0;
	
	while (1) {
	
		if (str1[i] == 0 || str2[i] == 0)
			break;
			
		if (str1[i] != str2[i])
			break;
		
		i++;
	}
	
	return i;
}

char *path_rem(char *path, char *dev) {
	int diff = strdiff(path, dev);
	return &path[diff];
}















