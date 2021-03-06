#pragma once

#include <stdint.h>
#include <console.h>

#define ALIGN_RIGHT(v, a)	\
	(((v) + (a) - 1) & ~((a) - 1))

#define F_INTERRUPTS		(1 << 9)

uint32_t read_ebp();
uint32_t read_esp();
uint32_t read_eip();
uint32_t read_cr2();
uint32_t read_ret();
uint32_t read_flags();

void halt();
void stop();

#define CLOCKS_PER_SEC 		(10)

typedef unsigned clock_t;

clock_t clock();

void sleep(uintmax_t millis);

#define panic(args...) \
	(kprintfln(args), stop())

void enable_ints();
void disable_ints();

int enter_critical_section();
void exit_critical_section(int interrupt_state);

char *kstrdup(char *str);

const char *kstrchr(const char *str, int character);
int strdiff(const char *str1, const char *str2);
char *path_rem(char *path, char *dev);

int cas(int *ptr, int oldval, int newval);

#define NAME(m) #m		
		
#define ASSERT(ex, msg) 	\
	if (!(ex)) {			\
		panic((msg));		\
	}
		
#define ASSERT_PAGE_ALIGNED(addr) 				\
	ASSERT(										\
		((addr) % PAGE_SIZE) == 0,	\
		#addr " is not page aligned!")	
		
#define ASSERT_ALIGNMENT(v, a) 			\
	ASSERT(								\
		((v) % (a)) == 0,	\
		#v " is not aligned to " #a)	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		