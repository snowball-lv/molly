#include <idt.h>
#include <console.h>
#include <types.h>
#include <gdt.h>
#include <isr.h>
#include <stdlib.h>

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

#define WRAPPER_NAME(num) isr_wrapper_##num
#define WRAPPER_DECL(num) extern void WRAPPER_NAME(num)()

WRAPPER_DECL(0);
WRAPPER_DECL(1);
WRAPPER_DECL(2);
WRAPPER_DECL(3);
WRAPPER_DECL(4);
WRAPPER_DECL(5);
WRAPPER_DECL(6);
WRAPPER_DECL(7);

WRAPPER_DECL(8);
WRAPPER_DECL(9);
WRAPPER_DECL(10);
WRAPPER_DECL(11);
WRAPPER_DECL(12);
WRAPPER_DECL(13);
WRAPPER_DECL(14);
WRAPPER_DECL(15);

WRAPPER_DECL(16);
WRAPPER_DECL(17);
WRAPPER_DECL(18);
WRAPPER_DECL(19);
WRAPPER_DECL(20);
WRAPPER_DECL(21);
WRAPPER_DECL(22);
WRAPPER_DECL(23);

WRAPPER_DECL(24);
WRAPPER_DECL(25);
WRAPPER_DECL(26);
WRAPPER_DECL(27);
WRAPPER_DECL(28);
WRAPPER_DECL(29);
WRAPPER_DECL(30);
WRAPPER_DECL(31);

void init_idt() {
	
	ASSERT_SIZE(IDTR, 		6);
	ASSERT_SIZE(IDTDesc, 	8);
		
	idt_set_gate(0, (uint32_t)WRAPPER_NAME(0));
	idt_set_gate(1, (uint32_t)WRAPPER_NAME(1));
	idt_set_gate(2, (uint32_t)WRAPPER_NAME(2));
	idt_set_gate(3, (uint32_t)WRAPPER_NAME(3));
	idt_set_gate(4, (uint32_t)WRAPPER_NAME(4));
	idt_set_gate(5, (uint32_t)WRAPPER_NAME(5));
	idt_set_gate(6, (uint32_t)WRAPPER_NAME(6));
	idt_set_gate(7, (uint32_t)WRAPPER_NAME(7));
	idt_set_gate(8, (uint32_t)WRAPPER_NAME(8));
	idt_set_gate(9, (uint32_t)WRAPPER_NAME(9));
	idt_set_gate(10, (uint32_t)WRAPPER_NAME(10));
	idt_set_gate(11, (uint32_t)WRAPPER_NAME(11));
	idt_set_gate(12, (uint32_t)WRAPPER_NAME(12));
	idt_set_gate(13, (uint32_t)WRAPPER_NAME(13));
	idt_set_gate(14, (uint32_t)WRAPPER_NAME(14));
	idt_set_gate(15, (uint32_t)WRAPPER_NAME(15));
	idt_set_gate(16, (uint32_t)WRAPPER_NAME(16));
	idt_set_gate(17, (uint32_t)WRAPPER_NAME(17));
	idt_set_gate(18, (uint32_t)WRAPPER_NAME(18));
	idt_set_gate(19, (uint32_t)WRAPPER_NAME(19));
	idt_set_gate(20, (uint32_t)WRAPPER_NAME(20));
	idt_set_gate(21, (uint32_t)WRAPPER_NAME(21));
	idt_set_gate(22, (uint32_t)WRAPPER_NAME(22));
	idt_set_gate(23, (uint32_t)WRAPPER_NAME(23));
	idt_set_gate(24, (uint32_t)WRAPPER_NAME(24));
	idt_set_gate(25, (uint32_t)WRAPPER_NAME(25));
	idt_set_gate(26, (uint32_t)WRAPPER_NAME(26));
	idt_set_gate(27, (uint32_t)WRAPPER_NAME(27));
	idt_set_gate(28, (uint32_t)WRAPPER_NAME(28));
	idt_set_gate(29, (uint32_t)WRAPPER_NAME(29));
	idt_set_gate(30, (uint32_t)WRAPPER_NAME(30));
	idt_set_gate(31, (uint32_t)WRAPPER_NAME(31));
	
	for (word i = 0; i < MAX_INTERRUPTS; i++)
		idt_set_gate(i, (uint32_t)_ISR_WRAPPERS[i]);
	
	_idtr.size = sizeof(IDTDesc) * MAX_INTERRUPTS - 1;
	_idtr.addr = (uint32_t)&_idt;
	
	lidt(&_idtr);
}











