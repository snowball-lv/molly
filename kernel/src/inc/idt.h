#pragma once

#include <stdint.h>

void init_idt();

//don't fuck around with this
typedef struct {

	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	
	uint32_t num;
	uint32_t err_code;
	
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	
	uint32_t uesp;
	uint32_t uss;
	
} trapframe_t;

typedef void (*isr_t)(trapframe_t *tf);

void set_isr(int num, isr_t isr);



















