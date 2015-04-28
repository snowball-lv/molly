#include <kbd.h>
#include <console.h>
#include <pic.h>
#include <idt.h>
#include <io.h>
#include <string.h>
#include <param.h>
#include <klib.h>
#include <acpi.h>

#define DATA_PORT 		0x60
#define STATUS_REG		0x64
#define COMMAND_REG		0x64

static char read_byte() {
	return in8(DATA_PORT);
}

static void kbd_isr(trapframe_t *tf) {
	char byte = read_byte();
	kprintfln("kbd event: %x", byte);
}

void init_kbd() {
	kprintfln("init kbd");
	
	//set kbd isr
	set_isr(IRQ_BASE + IRQ_KBD, kbd_isr);
	
	//enable kbd
	uint8_t mask = pic_read_data(PIC_MASTER);
	mask &= ~(1 << IRQ_KBD);
	pic_write_data(PIC_MASTER, mask);
}



















