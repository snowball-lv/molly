#include <pit.h>
#include <io.h>
#include <pic.h>
#include <console.h>
#include <klib.h>
#include <interrupt.h>
#include <string.h>
#include <proc.h>

#define PIT_HZ			1193182
#define COUNTER_0 		0x40
#define COUNTER_1		0x41
#define COUNTER_2 		0x42
#define COMMAND_WORD	0x43

static clock_t _pit_ticks;

clock_t pit_get_ticks() {
	return _pit_ticks;
}

static void tick_isr(trapframe_t *tf) {
	_pit_ticks++;
	kprintfln("tick: %d", _pit_ticks);
	//reschedule();
}

void init_pit() {

	//send control word
	out8(COMMAND_WORD, 0b00110110);
	
	size_t count = PIT_HZ / (float)CLOCKS_PER_SEC;
	
	//LSB
	out8(COUNTER_0, count & 0xff);
	
	//MSB
	out8(COUNTER_0, (count >> 8) & 0xff);
	
	//set timer isr
	isr_set_handler(32, tick_isr);
	
	//initialize ticks
	_pit_ticks = 0;
	
	//enable timer
	uint8_t mask = pic_read_data(PIC_MASTER);
	mask &= ~(1 << PIC_IRQ_TIMER);
	pic_write_data(PIC_MASTER, mask);
}










