#pragma once

#include <stdint.h>

#define PAGE_SIZE	4096

uint32_t read_ebp();
uint32_t read_esp();
uint32_t read_eip();

void halt();
void stop();

#define CLOCKS_PER_SEC 100

typedef uintmax_t clock_t;

clock_t clock();

void sleep(uintmax_t millis);

#define NAME(m) #m		
		
#define ASSERT(ex, msg) 	\
	if (!(ex)) {			\
		kprintfln(msg);		\
		stop();				\
	}

#define ASSERT_ALIGN(addr) 					\
	ASSERT(									\
		((uintptr_t)(addr) % 4096) == 0,	\
		#addr " is not 4k aligned!")

#define ASSERT_SIZE(type, size) 	\
	ASSERT(							\
		sizeof(type) == (size),		\
		#type " is wrong size!")	
		
		
		
		
		
		
		
		