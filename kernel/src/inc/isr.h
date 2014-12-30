#pragma once

#include <idt.h>

typedef void (*fptr)();
extern fptr _ISR_WRAPPERS[MAX_INTERRUPTS];
void set_isr(word id, void *handler);