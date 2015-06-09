#pragma once

#include <string.h>

#define EXIT_SUCCESS 	0
#define EXIT_FAILURE 	1

void *malloc(size_t size);
void free(void *ptr);
void exit(int status);