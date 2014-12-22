#include <interrupt.h>
#include <console.h>

IntDesc _idt[MAX_INTERRUPTS];
IDTR _idtr;

void handler() {
	printfln("*** unhandled exception!");
}


