#include <pic.h>
#include <io.h>

#define MASTER_COMMAND 	0x20
#define MASTER_DATA		0x21
#define SLAVE_COMMAND 	0xa0
#define SLAVE_DATA		0xa1

void initPIC() {

	//send Initialization Control Word 1
	//start initialization
	out8(MASTER_COMMAND, 0b00010001);
	out8(SLAVE_COMMAND, 0b00010001);

	//send Initialization Control Word 2
	//IRQ base address (0 - 31 are reserved)
	out8(MASTER_DATA, 32);
	out8(SLAVE_DATA, 40);
	
	//send Initialization Control Word 3
	//cascading
	out8(MASTER_DATA, 0b00000100);
	out8(SLAVE_DATA, 2);
	
	//send Initialization Control Word 4
	//x86 mode
	out8(MASTER_DATA, 0b00000001);
	out8(SLAVE_DATA, 0b00000001);
	
	//done. null the registers
	out8(MASTER_DATA, 0);
	out8(SLAVE_DATA, 0);
}


















