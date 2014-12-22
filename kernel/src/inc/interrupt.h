#pragma once

#include <types.h>

#define MAX_INTERRUPTS 256

#define ATTR __attribute__((packed))

typedef struct {
	u16 addrLow;
	u16 selector;
	u8 zero;
	u8 flags;
	u16 addrHigh;
} ATTR IntDesc;

typedef struct {
	u16 size;
	u32 addr;
} ATTR IDTR;

#undef ATTR

void lidt(IDTR *idtr);
void wrapper();

extern IntDesc _idt[MAX_INTERRUPTS];
extern IDTR _idtr;








