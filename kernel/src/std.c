
#include <string.h>

void *memcpy(void *destination, const void *source, size_t num) {
	char *dst = destination;
	const char *src = source;
	for (size_t i = 0; i < num; i++)
		dst[i] = src[i];
	return destination;
}

char *strcat(char *destination, const char *source) {
	char *dst = destination;
	dst += strlen(destination);
	strcpy(dst, source);
	return destination;
}












