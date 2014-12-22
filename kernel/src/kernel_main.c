#include <types.h>
#include <console.h>
#include <interrupt.h>

word kernel_main() {

	clear();
	printfln("executing kernel...");
	printfln("MAX_INTERRUPTS: %d", MAX_INTERRUPTS);
	printfln("sizeof(IntDesc): %d", sizeof(IntDesc));
	printfln("sizeof(IDTR): %d", sizeof(IDTR));
	
	u32 addr = (u32)&wrapper;
	for (word i = 0; i < MAX_INTERRUPTS; i++) {
		_idt[i].addrLow = addr & 0xffff;
		_idt[i].selector = 0x8;
		_idt[i].zero = 0;
		_idt[i].flags = 0b10001110;
		_idt[i].addrHigh = (addr >> 16) & 0xffff;
	}
	
	_idtr.size = sizeof(IntDesc) * MAX_INTERRUPTS - 1;
	_idtr.addr = (u32)&_idt;
	
	lidt(&_idtr);
	
	printfln("kernel exited");
	return 0xbabecafe;
}










