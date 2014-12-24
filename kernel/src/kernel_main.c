#include <types.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <stdlib.h>

word kernel_main() {

	clear();
	printfln("executing kernel...");
	
	initGDT();
	initIDT();
	initPIC();
	initPIT();
	
	__asm__("sti");
	
	printfln("kernel exited");
	return 0xbabecafe;
}














