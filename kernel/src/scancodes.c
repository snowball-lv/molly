#include <scancodes.h>
#include <console.h>
#include <string.h>

static int keymap_0[] = {};

static void sc_to_key(uint8_t *sc, key_event *e) {

	switch (sc[0]) {
	
	case 0xf0:
		if (sc[1] != 0) {
			e->key = keymap_0[sc[1]];
			e->event = E_BREAK;
		}
		return;
		
	case 0xe0:
		switch (sc[1]) {
		case 0:
			break;
		case 0xf0:
			return;
		}
	
	case 0xe1:
		return;
	}
	
	e->key = keymap_0[sc[0]];
	e->event = E_MAKE;
}

static uint8_t buff[9];
static uint8_t *ptr = &buff[0];
	
void get_key(int scancode, key_event *e) {

	*ptr = (uint8_t)scancode;
	ptr++;
	*ptr = 0;
	
	sc_to_key(buff, e);
}

char get_ascii(int key) {

	if (KEY_A <= key && key <= KEY_Z)
		return key;
		
	if (KEY_0 <= key && key <= KEY_9)
		return key;

	return 0;
}






























