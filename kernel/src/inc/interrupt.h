#pragma once

#include <types.h>
#include <idt.h>

typedef void (*isr_w_fptr)();
extern isr_w_fptr _ISR_WRAPPERS[MAX_INTERRUPTS];

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
	
} trapframe_t;

typedef void (*isr_fptr)(trapframe_t *tf);

void isr_add_handler(uint32_t num, isr_fptr ptr);














