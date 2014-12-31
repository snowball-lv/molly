#include <kbd.h>
#include <pic.h>
#include <isr.h>
#include <io.h>
#include <console.h>
#include <keyboard_xt.h>
#include <keys.h>

#define ENC_INPUT_BUF	0x60
#define ENC_COMMAND		0x60

#define CTRL_STATUS		0x64
#define CTRL_COMMAND	0x64

static u8 read_input() {
	return in8(ENC_INPUT_BUF);
}

static byte getChar(word key);

static void kbd_isr(word id) {

	u8 sc = read_input();
	
	KeyEvent e = {0};
	setEvent(&e, sc);
	
	if (e.type == EVENT_TYPE_MAKE) {
		byte c = getChar(e.key);
		if (c) {
			printf("%c", c);
		}
	}
}

void initKBD() {

	//set keyboard isr
	set_isr(IRQ_BASE + PIC_IRQ_KBD, kbd_isr);

	//enable keyboard
	u8 mask = pic_read_data(PIC_MASTER);
	mask &= ~(1 << PIC_IRQ_KBD);
	pic_write_data(PIC_MASTER, mask);
}

static byte getChar(word key) {

	switch (key) {
	
	case KEY_A: return 'a';
	case KEY_B: return 'b';
	case KEY_C: return 'c';
	case KEY_D: return 'd';
	case KEY_E: return 'e';
	case KEY_F: return 'f';
	case KEY_G: return 'g';
	case KEY_H: return 'h';
	case KEY_I: return 'i';
	case KEY_J: return 'j';
	case KEY_K: return 'k';
	case KEY_L: return 'l';
	case KEY_M: return 'm';
	case KEY_N: return 'n';
	case KEY_O: return 'o';
	case KEY_P: return 'p';
	case KEY_Q: return 'q';
	case KEY_R: return 'r';
	case KEY_S: return 's';
	case KEY_T: return 't';
	case KEY_U: return 'u';
	case KEY_V: return 'v';
	case KEY_W: return 'w';
	case KEY_X: return 'x';
	case KEY_Y: return 'y';
	case KEY_Z: return 'z';
	
	case KEY_ENTER: return '\n';
	case KEY_SPACE: return ' ';
	
	}
	return 0;
}










