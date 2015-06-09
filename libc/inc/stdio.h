#pragma once


#include <string.h>


#define EOF		(-1)


typedef struct {

	int fd;

} FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
size_t fwrite (const void *ptr, size_t size, size_t count, FILE *stream);

int printf(const char *format, ... );

int fileno(FILE *stream);