#include <idt.h>
#include <console.h>
#include <types.h>
#include <gdt.h>
#include <isr.h>

#define ATTR __attribute__((packed))

typedef struct {
	u16 size;
	u32 addr;
} ATTR IDTR;

typedef struct {
	u16 addrLow;
	u16 selector;
	u8 zero;
	u8 flags;
	u16 addrHigh;
} ATTR IDTDesc;

static IDTR 	_idtr;
static IDTDesc 	_idt[MAX_INTERRUPTS];

//idt_asm.asm
void lidt(IDTR *idtr);

static void set_gate(word id, u32 isr) {
	_idt[id].addrLow = isr & 0xffff;
	_idt[id].selector = CODE_SEL;
	_idt[id].zero = 0;
	_idt[id].flags = 0b10001110;
	_idt[id].addrHigh = (isr >> 16) & 0xffff;
}

void initIDT() {

	printfln("MAX_INTERRUPTS: %d", MAX_INTERRUPTS);
	printfln("sizeof(IDTR): %d", sizeof(IDTR));
	printfln("sizeof(IDTDesc): %d", sizeof(IDTDesc));
	
	for (word i = 0; i < MAX_INTERRUPTS; i++)
		set_gate(i, (u32)_ISR_WRAPPERS[i]);
	
	_idtr.size = sizeof(IDTDesc) * MAX_INTERRUPTS - 1;
	_idtr.addr = (u32)&_idt;
	
	lidt(&_idtr);
}











