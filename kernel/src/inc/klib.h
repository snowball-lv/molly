#pragma once

#include <stdint.h>

uint32_t read_ebp();
uint32_t read_esp();
uint32_t read_eip();

void halt();
void stop();

#define CLOCKS_PER_SEC 100

typedef unsigned clock_t;

clock_t clock();

void sleep(uintmax_t millis);

void panic(const char *msg);

void enable_ints();
void disable_ints();

#define NAME(m) #m		
		
#define ASSERT(ex, msg) 	\
	if (!(ex)) {			\
		panic((msg));		\
	}

#define ASSERT_SIZE(type, size) 	\
	ASSERT(							\
		sizeof(type) == (size),		\
		#type " is wrong size!")	
		
#define ASSERT_PAGE_ALIGNED(addr) 				\
	ASSERT(										\
		((addr) % PAGE_SIZE) == 0,	\
		#addr " is not page aligned!")	
		
#define ASSERT_ALIGNMENT(v, a) 			\
	ASSERT(								\
		((v) % (a)) == 0,	\
		#v " is not aligned to " #a)	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		