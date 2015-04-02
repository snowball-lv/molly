
#include <string.h>

void *memcpy(void *destination, const void *source, size_t num) {
	char *dst = destination;
	const char *src = source;
	for (size_t i = 0; i < num; i++)
		dst[i] = src[i];
	return destination;
}

void *memset(void *ptr, int value, size_t num) {
	for (size_t i = 0; i < num; i++)
		((unsigned char *)ptr)[i] = (unsigned char)value;
	return ptr;
}

char *strcpy(char *destination, const char *source) {
	char *dst = destination;
	while (*source) {
		*dst = *source;
		dst++;
		source++;
	}
	return destination;
}

char *strcat(char *destination, const char *source) {
	char *dst = destination;
	dst += strlen(destination);
	strcpy(dst, source);
	return destination;
}

size_t strlen(const char *str) {
	size_t c = 0;
	while (*str) {
		c++;
		str++;
	}
	return c;
}

char *strchr(const char *str, int character) {

	while (*str != character && *str != 0)
		str++;

	return (char *)str;
}














