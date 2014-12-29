#pragma once

#include <types.h>

void copy(void *src, void *dst, uword num);

void memset(void *ptr, byte value, uword num);

typedef ubyte *va_list;
	
#define	STACK_SIZE(type) \
	(((sizeof(type) + sizeof(word) - 1) / \
	sizeof(word)) * sizeof(word))

#define	va_start(ap, last) \
	(ap = ((va_list)&last) + STACK_SIZE(last))

#define va_arg(ap, type) \
	(ap += STACK_SIZE(type), \
	*((type *)(ap - STACK_SIZE(type))))

#define va_end(ap)

#define NAME(m) #m