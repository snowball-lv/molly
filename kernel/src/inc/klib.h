#pragma once

#include <types.h>

#define PAGE_SIZE	4096

uint32_t read_ebp();
uint32_t read_esp();
uint32_t read_eip();

void halt();
void stop();

void memset(void *ptr, byte value, size_t num);

#define CLOCKS_PER_SEC 100

typedef uword clock_t;

clock_t clock();
		
void copy(void *src, void *dst, uword num);

void sleep(uword millis);

size_t strlen(const byte *str);

typedef byte *va_list;
	
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
		
#define ASSERT(ex, msg) 	\
	if (!(ex)) {			\
		kprintfln(msg);		\
		stop();				\
	}

#define ASSERT_ALIGN(addr) 				\
	ASSERT(								\
		((addr_t)(addr) % 4096) == 0,	\
		#addr " is not 4k aligned!")

#define ASSERT_SIZE(type, size) 	\
	ASSERT(							\
		sizeof(type) == (size),		\
		#type " is wrong size!")	
		
		
		
		
		
		
		
		