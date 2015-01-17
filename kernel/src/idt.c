#include <idt.h>
#include <console.h>
#include <types.h>
#include <gdt.h>
#include <klib.h>
#include <interrupt.h>

#define ATTR __attribute__((packed))

typedef struct {
	uint16_t size;
	uint32_t addr;
} ATTR IDTR;

typedef struct {
	uint16_t 	addr_low;
	uint16_t 	selector;
	uint8_t 	zero;
	uint8_t 	flags;
	uint16_t 	addr_high;
} ATTR IDTDesc;

static IDTR 	_idtr;
static IDTDesc 	_idt[MAX_INTERRUPTS];

//idt_asm.asm
void lidt(IDTR *idtr);

static void idt_set_gate(size_t num, uint32_t isr) {

	IDTDesc *desc = &_idt[num];
	
	desc->addr_low 	= isr & 0xffff;
	desc->selector 	= CODE_SEL;
	desc->zero 		= 0;
	desc->flags 	= 0b10001110;
	desc->addr_high = (isr >> 16) & 0xffff;
}

void init_idt() {
	
	ASSERT_SIZE(IDTR, 		6);
	ASSERT_SIZE(IDTDesc, 	8);
		
	for (size_t i = 0; i < MAX_INTERRUPTS; i++)
		idt_set_gate(i, (uint32_t)_ISR_WRAPPERS[i]);
	
	_idtr.size = sizeof(IDTDesc) * MAX_INTERRUPTS - 1;
	_idtr.addr = (uint32_t)&_idt;
	
	lidt(&_idtr);
}




















