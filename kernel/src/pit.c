#include <pit.h>
#include <io.h>
#include <pic.h>
#include <console.h>
#include <klib.h>
#include <string.h>
#include <idt.h>
#include <acpi.h>

#define PIT_HZ			1193182
#define COUNTER_0 		0x40
#define COUNTER_1		0x41
#define COUNTER_2 		0x42
#define COMMAND_WORD	0x43

static clock_t PIT_TICKS;

clock_t ticks() {
	return PIT_TICKS;
}

static void tick_isr(trapframe_t *tf) {
	PIT_TICKS++;
}

void init_pit() {

	//send control word
	out8(COMMAND_WORD, 0b00110110);
	
	size_t count = PIT_HZ / CLOCKS_PER_SEC;
	
	//LSB
	out8(COUNTER_0, count & 0xff);
	
	//MSB
	out8(COUNTER_0, (count >> 8) & 0xff);
	
	//set timer isr
	set_isr(IRQ_BASE + IRQ_TIMER, tick_isr);
	
	//enable timer
	uint8_t mask = pic_read_data(PIC_MASTER);
	mask &= ~(1 << IRQ_TIMER);
	pic_write_data(PIC_MASTER, mask);
}










