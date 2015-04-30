
#include <string.h>

void *memcpy(void *destination, const void *source, size_t num) {
	char *dst = destination;
	const char *src = source;
	for (size_t i = 0; i < num; i++)
		dst[i] = src[i];
	return destination;
}

char *strcpy(char *destination, const char *source) {
	char *dst = destination;
	while (*source) {
		*dst = *source;
		dst++;
		source++;
	}
	*dst = 0;
	return destination;
}

char *strcat(char *destination, const char *source) {
	char *dst = destination;
	dst += strlen(destination);
	strcpy(dst, source);
	return destination;
}

int strcmp(const char *str1, const char *str2) {

	size_t len = strlen(str1);
	if (len != strlen(str2))
		return 1;
		
	for (size_t i = 0; i < len; i++) {
		if (str1[i] != str2[i])
			return 1;
	}
	
	return 0;
}














