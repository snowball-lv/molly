#include <kbd.h>
#include <pic.h>
#include <isr.h>
#include <io.h>
#include <console.h>

#define ENC_INPUT_BUF	0x60
#define ENC_COMMAND		0x60

#define CTRL_STATUS		0x64
#define CTRL_COMMAND	0x64

static u8 read_input() {
	return in8(ENC_INPUT_BUF);
}

static void kbd_isr(word id) {
	u8 sc = read_input();
	printfln("%x", sc);
}

void initKBD() {

	//set keyboard isr
	set_isr(IRQ_BASE + PIC_IRQ_KBD, kbd_isr);

	//enable keyboard
	u8 mask = pic_read_data(PIC_MASTER);
	mask &= ~(1 << PIC_IRQ_KBD);
	pic_write_data(PIC_MASTER, mask);
}