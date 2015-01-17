#pragma once

#include <types.h>

#define PAGE_SIZE	4096

uint32_t read_ebp();
uint32_t read_esp();
uint32_t read_eip();

void halt();
void stop();

void memset(void *ptr, byte value, size_t num);

#define ASSERT(ex, msg) 	\
	if (!(ex)) {			\
		printfln(msg);		\
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
		
		
		
		
		
		
		
		
		
		
		
		