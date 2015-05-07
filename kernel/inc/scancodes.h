#pragma once

#include <stdint.h>

#define E_NONE		0
#define E_MAKE		1
#define E_BREAK		2

typedef struct {
	int key;
	int event;
} key_event;

void get_key(int scancode, key_event *e);
char get_ascii(int key);

#define KEY_NONE	0
#define KEY_BAD		0

#define KEY_A	'a'
#define KEY_B	'b'
#define KEY_C	'c'
#define KEY_D	'd'
#define KEY_E	'e'
#define KEY_F	'f'
#define KEY_G	'g'
#define KEY_H	'h'
#define KEY_I	'i'
#define KEY_J	'j'
#define KEY_K	'k'
#define KEY_L	'l'
#define KEY_M	'm'
#define KEY_N	'n'
#define KEY_O	'o'
#define KEY_P	'p'
#define KEY_Q	'q'
#define KEY_R	'r'
#define KEY_S	's'
#define KEY_T	't'
#define KEY_U	'u'
#define KEY_V	'v'
#define KEY_W	'w'
#define KEY_X	'x'
#define KEY_Y	'y'
#define KEY_Z	'z'

#define KEY_0	'0'
#define KEY_1	'1'
#define KEY_2	'2'
#define KEY_3	'3'
#define KEY_4	'4'
#define KEY_5	'5'
#define KEY_6	'6'
#define KEY_7	'7'
#define KEY_8	'8'
#define KEY_9	'9'

#define KEY_GRAVE_ACCENT	'`'
#define KEY_HYPHEN			'-'
#define KEY_EQUALS			'='
#define KEY_B_SLASH			'\\'
#define KEY_BKSP			'\b'
#define KEY_SPACE			' '
#define KEY_TAB				'\t'
#define KEY_SEMICOLON		';'
#define KEY_APOSTROPHE		'\''
#define KEY_COMMA			','
#define KEY_PERIOD			'.'
#define KEY_F_SLASH			'/'
#define KEY_L_BRACKET		'['
#define KEY_R_BRACKET		']'
#define KEY_CAPS			0

#define KEY_L_SHFT	0
#define KEY_L_CTRL	0
#define KEY_L_GUI	0
#define KEY_L_ALT	0

#define KEY_R_SHFT	0
#define KEY_R_CTRL	0
#define KEY_R_GUI	0
#define KEY_R_ALT	0

#define KEY_R_APPS		0
#define KEY_R_ENTER		'\n'
#define KEY_R_ESC		0

#define KEY_F1		0
#define KEY_F2		0
#define KEY_F3		0
#define KEY_F4		0
#define KEY_F5		0
#define KEY_F6		0
#define KEY_F7		0
#define KEY_F8		0
#define KEY_F9		0
#define KEY_F10		0
#define KEY_F11		0
#define KEY_F12		0

#define KEY_PRNT_SCRN	0
#define KEY_SCROLL		0
#define KEY_PAUSE		0

#define KEY_INSERT		0
#define KEY_HOME		0
#define KEY_PG_UP		0
#define KEY_PG_DN		0
#define KEY_DELETE		0
#define KEY_END			0

#define KEY_U_ARROW		0
#define KEY_L_ARROW		0
#define KEY_D_ARROW		0
#define KEY_R_ARROW		0

#define KEY_NUM			0

#define KEY_KP_F_SLASH		0
#define KEY_KP_ASTERISK		0
#define KEY_KP_HYPHEN		0
#define KEY_KP_PLUS			0
#define KEY_KP_ENTER		0
#define KEY_KP_PERIOD		0

#define KEY_KP_0	0
#define KEY_KP_1	0
#define KEY_KP_2	0
#define KEY_KP_3	0
#define KEY_KP_4	0
#define KEY_KP_5	0
#define KEY_KP_6	0
#define KEY_KP_7	0
#define KEY_KP_8	0
#define KEY_KP_9	0

#define KEY_ACPI_POWER		0
#define KEY_ACPI_SLEEP		0
#define KEY_ACPI_WAKE		0

#define KEY_WIN_NEXT_TRACK		0
#define KEY_WIN_PREVIOUS_TRACK	0
#define KEY_WIN_STOP			0
#define KEY_WIN_PLAY_PAUSE		0
#define KEY_WIN_PLAY_MUTE		0
#define KEY_WIN_VOLUME_UP		0
#define KEY_WIN_VOLUME_DOWN		0
#define KEY_WIN_MEDIA_SELECT	0
#define KEY_WIN_E_MAIL			0
#define KEY_WIN_CALCULATOR		0
#define KEY_WIN_MY_COMPUTER		0
#define KEY_WIN_WWW_SEARCH		0
#define KEY_WIN_WWW_HOME		0
#define KEY_WIN_WWW_BACK		0
#define KEY_WIN_WWW_FORWARD		0
#define KEY_WIN_WWW_STOP		0
#define KEY_WIN_WWW_REFRESH		0
#define KEY_WIN_WWW_FAVORITES	0






















