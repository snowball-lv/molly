#include <pit.h>
#include <io.h>
#include <time.h>
#include <pic.h>
#include <idt.h>
#include <console.h>

#define PIT_HZ			1193182
#define COUNTER_0 		0x40
#define COUNTER_1		0x41
#define COUNTER_2 		0x42
#define COMMAND_WORD	0x43

//pit_asm.asm
void pit_isr_wrapper();

static clock_t _ticks;

void initPIT() {

	//send control word
	out8(COMMAND_WORD, 0b00110110);
	
	word count = PIT_HZ / CLOCKS_PER_SEC;
	
	//LSB
	out8(COUNTER_0, count & 0xff);
	
	//MSB
	out8(COUNTER_0, (count >> 8) & 0xff);
	
	//set timer isr
	set_gate(IRQ_BASE + PIC_IRQ_TIMER, (u32)&pit_isr_wrapper);
	
	//initialize ticks
	_ticks = 0;
	
	//enable timer
	u8 mask = pic_read_data(PIC_MASTER);
	mask &= ~(1 << PIC_IRQ_TIMER);
	pic_write_data(PIC_MASTER, mask);
}

void pit_isr() {
	_ticks++;
}

clock_t clock() {
	return _ticks;
}










