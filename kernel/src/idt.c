#include <idt.h>
#include <console.h>
#include <types.h>
#include <gdt.h>

#define MAX_INTERRUPTS 256
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
void wrapper();

void initIDT() {

	printfln("MAX_INTERRUPTS: %d", MAX_INTERRUPTS);
	printfln("sizeof(IDTR): %d", sizeof(IDTR));
	printfln("sizeof(IDTDesc): %d", sizeof(IDTDesc));
	
	u32 addr = (u32)&wrapper;
	for (word i = 0; i < MAX_INTERRUPTS; i++) {
		_idt[i].addrLow = addr & 0xffff;
		_idt[i].selector = CODE_SEL;
		_idt[i].zero = 0;
		_idt[i].flags = 0b10001110;
		_idt[i].addrHigh = (addr >> 16) & 0xffff;
	}
	
	_idtr.size = sizeof(IDTDesc) * MAX_INTERRUPTS - 1;
	_idtr.addr = (u32)&_idt;
	
	lidt(&_idtr);
}

//called from wrapper() in idt_asm.asm
void handler() {
	printfln("*** unhandled exception!");
}








