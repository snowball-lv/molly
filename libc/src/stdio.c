#include <stdio.h>
#include <stdlib.h>
#include <molly.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>


FILE *stdin;
FILE *stdout;
FILE *stderr;


FILE *fopen(const char *filename, const char *mode) {

	int fd = open((char *)filename);

	if (fd < 0)
		return 0;

	FILE *f = malloc(sizeof(FILE));

	f->fd = fd;

	return f;
}


int fclose(FILE *stream) {

	int rc = close(stream->fd);

	free(stream);

	return (rc == 0) ? 0 : EOF;
}


size_t fread(void *ptr, size_t size, size_t count, FILE *stream) {

	if (size == 0 || count == 0)
		return 0;

	char *cptr = ptr;
	size_t elems = 0;

	for (; elems < count; elems++) {

		int rc = read(stream->fd, cptr, size);
		
		if (rc <= 0)
			break;

		cptr += size;
	}

	return elems;
}



size_t fwrite (const void *ptr, size_t size, size_t count, FILE *stream) {

	if (size == 0 || count == 0)
		return 0;

	char *cptr = ptr;
	size_t elems = 0;

	for (; elems < count; elems++) {

		int rc = write(stream->fd, cptr, size);
		
		if (rc != size)
			break;

		cptr += size;
	}

	return elems;
}

int putchar(int character) {
	fwrite((char[]){character}, 1, 1, stdout);
	return character;
}

int puts(const char *str) {
	while (*str != 0)
		putchar(*str++);
	return 1;
}

static void printDec(int value) {
	char buffer[(sizeof(int) * 8) + 1];
	char *str = itoa(value, buffer, 10);
	puts(str);
}

static void printHex(int value) {
	char buffer[(sizeof(int) * 8) + 1];
	char *str = itoa(value, buffer, 16);
	puts("0x");
	puts(str);
}

static void printBin(int value) {
	char buffer[(sizeof(int) * 8) + 1];
	char *str = itoa(value, buffer, 2);
	puts(str);
	puts("b");
}

int printf(const char *format, ...) {
	
	va_list args;
    va_start(args, format);
	
	const char *str = format;

	for (size_t i = 0; str[i] != 0; i++) {
		if (str[i] == '%') {
		
			switch (str[i + 1]) {
			case 'd':
				printDec(va_arg(args, int));
				i++;
				break;
			case 'c':
				putchar(va_arg(args, int));
				i++;
				break;
			case 'x':
				printHex(va_arg(args, int));
				i++;
				break;
			case 'b':
				printBin(va_arg(args, int));
				i++;
				break;
			case 's':
				puts(va_arg(args, char *));
				i++;
				break;
			default:
				putchar(str[i]);
				break;
			}
			
		} else {
			putchar(str[i]);
		}
	}
	
	va_end(args);
	return 1;
}

int fileno(FILE *stream) {
	return stream->fd;
}

char *gets(char *str) {

	int fd = fileno(stdin);
	char *ptr = str;

	int total = 0;
	int count = 0;

	while ((count = read(fd, ptr, 4096)) > 0) {
		total += count;

		//check if last charater is new line
		if (ptr[count - 1] == '\n') {
			//replace new line with null
			ptr[count - 1] = 0;
			break;
		}

		ptr += count;
	}

	//error
	if (count == -1)
		return 0;

	return (total > 0) ? str : 0;
}