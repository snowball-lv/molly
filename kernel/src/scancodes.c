#include <scancodes.h>
#include <console.h>
#include <string.h>

static int sc_to_key_map[] = {

	[0x1c] = KEY_A,
	[0x32] = KEY_B,
	[0x21] = KEY_C,
	[0x23] = KEY_D,
	[0x24] = KEY_E,
	[0x2b] = KEY_F,
	[0x34] = KEY_G,
	[0x33] = KEY_H,
	[0x43] = KEY_I,
	[0x3b] = KEY_J,
	[0x42] = KEY_K,
	[0x4b] = KEY_L,
	[0x3a] = KEY_M,
	[0x31] = KEY_N,
	[0x44] = KEY_O,
	[0x4d] = KEY_P,
	[0x15] = KEY_Q,
	[0x2d] = KEY_R,
	[0x1b] = KEY_S,
	[0x2c] = KEY_T,
	[0x3c] = KEY_U,
	[0x2a] = KEY_V,
	[0x1d] = KEY_W,
	[0x22] = KEY_X,
	[0x35] = KEY_Y,
	[0x1a] = KEY_Z,
	
	[0x45] = KEY_0,
	[0x16] = KEY_1,
	[0x1e] = KEY_2,
	[0x26] = KEY_3,
	[0x25] = KEY_4,
	[0x2e] = KEY_5,
	[0x36] = KEY_6,
	[0x3d] = KEY_7,
	[0x3e] = KEY_8,
	[0x46] = KEY_9,
	
	[0x0e] = KEY_GRAVE_ACCENT,
	[0x4e] = KEY_HYPHEN,
	[0x55] = KEY_EQUALS,
	[0x5d] = KEY_B_SLASH,
	[0x66] = KEY_BKSP,
	[0x29] = KEY_SPACE,
	[0x0d] = KEY_TAB,
	[0x4c] = KEY_SEMICOLON,
	[0x52] = KEY_APOSTROPHE,
	[0x41] = KEY_COMMA,
	[0x49] = KEY_PERIOD,
	[0x4a] = KEY_F_SLASH,
	[0x54] = KEY_L_BRACKET,
	[0x5b] = KEY_R_BRACKET,
	
	[0x5a] = KEY_ENTER,
};
	
static int in_break = 0;

void get_key(int scancode, key_event *e) {

	if (!in_break && scancode == 0xf0) {
		e->key = KEY_NONE;
		e->event = E_NONE;
		in_break = 1;
		return;
	}
	
	e->key = sc_to_key_map[scancode];
	e->event = in_break ? E_BREAK : E_MAKE;
	in_break = 0;
}

char get_ascii(int key) {

	if (KEY_A <= key && key <= KEY_Z)
		return key;
		
	if (KEY_0 <= key && key <= KEY_9)
		return key;

	switch (key) {
		case KEY_GRAVE_ACCENT:
		case KEY_HYPHEN:
		case KEY_EQUALS:
		case KEY_B_SLASH:
		case KEY_BKSP:
		case KEY_SPACE:
		case KEY_TAB:
		case KEY_SEMICOLON:
		case KEY_APOSTROPHE:
		case KEY_COMMA:
		case KEY_PERIOD:
		case KEY_F_SLASH:
		case KEY_L_BRACKET:
		case KEY_R_BRACKET:
		case KEY_ENTER:
			return key;
	}
		
	return 0;
}




























