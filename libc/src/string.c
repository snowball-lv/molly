#include <string.h>
#include <stdlib.h>

size_t strlen(const char *str) {
	size_t c = 0;
	while (*str) {
		c++;
		str++;
	}
	return c;
}

char *strchr(const char *str, int character) {

	while (*str != 0 && *str != character)
		str++;
		
	if (*str == 0 && character != 0)
		return 0;
	
	return (char *)str;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num) {

	const char *a = ptr1;
	const char *b = ptr2;
	
	for (size_t i = 0; i < num; i++) {
		if (a[i] != b[i])
			return 1;
	}
	
	return 0;
}

size_t strcspn(const char *str1, const char *str2) {

	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);
	
	for (size_t i = 0; i < len1; i++) {
		for (size_t k = 0; k < len2; k++) {
			if (str1[i] == str2[k])
				return i;
		}
	}
	
	return len1;
}

void *memset(void *ptr, int value, size_t num) {
	for (size_t i = 0; i < num; i++)
		((unsigned char *)ptr)[i] = (unsigned char)value;
	return ptr;
}

char *strdup(const char *str) {
	size_t len = strlen(str);
	char *dup = malloc(len + 1);
	strcpy(dup, str);
	return dup;
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















