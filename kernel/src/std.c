
#include <string.h>

void *memcpy(void *destination, const void *source, size_t num) {
	char *dst = destination;
	const char *src = source;
	for (size_t i = 0; i < num; i++)
		dst[i] = src[i];
	return destination;
}

void *memset (void *ptr, int value, size_t num) {
	for (size_t i = 0; i < num; i++)
		((unsigned char *)ptr)[i] = (unsigned char)value;
	return ptr;
}


















