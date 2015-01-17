#pragma once

#include <types.h>
#include <idt.h>

typedef void (*isr_w_fptr)();
extern isr_w_fptr _ISR_WRAPPERS[MAX_INTERRUPTS];

//don't fuck around with this
typedef struct isr_info_t {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t num, err_code;
	uint32_t eip, cs, eflags;
} isr_info_t;

typedef void (*isr_fptr)(isr_info_t *info);

void isr_add_handler(uint32_t num, isr_fptr ptr);






















