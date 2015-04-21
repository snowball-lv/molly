#include <string.h>

size_t strlen(const char *str) {
	size_t c = 0;
	while (*str) {
		c++;
		str++;
	}
	return c;
}