#include <types.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>

word kernel_main() {

	clear();
	printfln("executing kernel...");
	
	initGDT();
	initIDT();
	initPIC();
	
	printfln("kernel exited");
	return 0xbabecafe;
}










