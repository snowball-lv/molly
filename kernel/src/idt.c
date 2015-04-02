#include <idt.h>
#include <console.h>
#include <gdt.h>
#include <klib.h>
#include <interrupt.h>
#include <string.h>
#include <param.h>

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

_Static_assert(sizeof(IDTR) 	== 6, "IDTR size not 6");
_Static_assert(sizeof(IDTDesc)	== 8, "IDTDesc size not 8");

static IDTR 	_idtr;
static IDTDesc 	_idt[MAX_INTERRUPTS];

//idt_asm.asm
void lidt(IDTR *idtr);

static void idt_set_gate(size_t num, uint32_t isr) {

	IDTDesc *desc = &_idt[num];
	
	desc->addr_low 	= isr & 0xffff;
	desc->selector 	= CODE_SEL;
	desc->zero 		= 0;
	desc->flags 	= 0b11101110;
	desc->addr_high = (isr >> 16) & 0xffff;
}

#define I_PAGE_FAULT	(14)

#define PF_P	(1 << 0)
#define PF_W	(1 << 1)
#define PF_U	(1 << 2)
#define PF_R	(1 << 3)
#define PF_I	(1 << 4)

static void pf_handler(trapframe_t *tf) {

	kprintfln("*** %x", read_cr2());
	
	kprintfln("- %s", tf->err_code & (1 << 0) ? 
		"caused by a protection violation" :
		"caused by a non-present page");
	
	kprintfln("- %s", tf->err_code & (1 << 1) ? 
		"write access caused the fault" :
		"read access caused the fault");
	
	kprintfln("- %s", tf->err_code & (1 << 2) ? 
		"the fault occurred in user mode" :
		"the fault occurred in supervisor  mode");
	
	kprintfln("- %s", tf->err_code & (1 << 3) ? 
		"one or more page directory entries contain reserved bits which are set to 1" :
		"This only applies when the PSE or PAE flags in CR4 are set to 1");
	
	kprintfln("- %s", tf->err_code & (1 << 4) ? 
		"the fault was caused by an instruction fetch" :
		"This only applies when the No-Execute bit is supported and enabled");
	
	panic("*** page fault");
}

static void gpf_handler(trapframe_t *tf) {
	kprintfln("*** eip: %x", tf->eip);
	kprintfln("*** seg: %x", tf->err_code);
	panic("*** General Protection Fault");
}

#define I_GP_FAULT 		(13)

void init_idt() {
		
	for (size_t i = 0; i < MAX_INTERRUPTS; i++)
		idt_set_gate(i, (uint32_t)_ISR_WRAPPERS[i]);
	
	_idtr.size = sizeof(IDTDesc) * MAX_INTERRUPTS - 1;
	_idtr.addr = (uint32_t)&_idt;
	
	lidt(&_idtr);
	
	isr_set_handler(I_PAGE_FAULT, pf_handler);
	isr_set_handler(I_GP_FAULT, gpf_handler);
}




















