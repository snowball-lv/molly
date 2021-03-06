#include <pic.h>
#include <io.h>

#define MASTER_COMMAND 	0x20
#define MASTER_DATA		0x21
#define SLAVE_COMMAND 	0xa0
#define SLAVE_DATA		0xa1

#define READ_ISR	0x0b 


uint8_t pic_isr(uint8_t pic) {

	uint16_t port = (pic == PIC_MASTER) ?
		MASTER_COMMAND :
		SLAVE_COMMAND;
		
	out8(port, READ_ISR);
	return in8(port);
}

void init_pic() {

	//send Initialization Control Word 1
	//start initialization
	out8(MASTER_COMMAND, 0b00010001);
	out8(SLAVE_COMMAND, 0b00010001);

	//send Initialization Control Word 2
	//IRQ base address (0 - 31 are reserved)
	out8(MASTER_DATA, IRQ_BASE);
	out8(SLAVE_DATA, IRQ_BASE + 8);
	
	//send Initialization Control Word 3
	//cascading
	out8(MASTER_DATA, 0b00000100);
	out8(SLAVE_DATA, 2);
	
	//send Initialization Control Word 4
	//x86 mode
	out8(MASTER_DATA, 0b00000001);
	out8(SLAVE_DATA, 0b00000001);
	
	//disable all IRQs
	pic_write_data(PIC_MASTER, 0xff);
	pic_write_data(PIC_SLAVE, 0xff);
}

uint8_t pic_read_data(uint8_t pic) {

	uint16_t port = (pic == PIC_MASTER) ?
		MASTER_DATA :
		SLAVE_DATA;
	
	return in8(port);
}

void pic_write_data(uint8_t pic, uint8_t data) {

	uint16_t port = (pic == PIC_MASTER) ?
		MASTER_DATA :
		SLAVE_DATA;
		
	out8(port, data);
}

void pic_eoi(uint8_t pic) {

	uint16_t port = (pic == PIC_MASTER) ?
		MASTER_COMMAND :
		SLAVE_COMMAND;
		
	out8(port, 0x20);
}










