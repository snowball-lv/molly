#include <vga.h>
#include <types.h>

#define VGA_MEM 0xb8000
#define NL '\n'

#define VGA_BLACK 		0x0
#define VGA_LIGHT_GRAY 	0x7

static u8 attrib = (VGA_BLACK << 4) | VGA_LIGHT_GRAY;

static u8 xPos = 0;
static u8 yPos = 0;

static void putc(s8 character);

void clear() {
	xPos = 0;
	yPos = 0;
	for (int i = 0; i < 25 * 80; i++)
		putc(' ');
	xPos = 0;
	yPos = 0;
}

static void scroll() {
	for (int y = 1; y < 25; y++) {
		u16 *row = (u16 *)(VGA_MEM + (y * 80) * 2);
		for (u16 *x = row; x < row + 80; x++)
			*(x - 80) = *x;
	}
	yPos = 24;
	xPos = 0;
	for (int i = 0; i < 80; i++)
		putc(' ');
	yPos = 24;
	xPos = 0;
}

static void prepare() {
	if (xPos == 80) {
		xPos = 0;
		yPos++;
	}
	if (yPos == 25)
		scroll();
}

static void putc(s8 character) {

	if (character == NL) {
		yPos++;
		xPos = 0;
		prepare();
		return;
	}
	
	prepare();
	u16 value = attrib << 8 | character;
	u16 *addr = (u16 *)(VGA_MEM + (yPos * 80 + xPos) * 2);
	*addr = value;
	xPos++;
}

void print(const s8 *str) {
	while (*str != 0)
		putc(*str++);
}

void println(const s8 *str) {
	print(str);
	putc(NL);
}

void printU32(u32 value) {

	u32 max = 1000000000;
	u8 notZero = 0;
	
	do {
		u8 d = value / max;
		notZero |= d;
		
		if (d || notZero)
			putc('0' + d);
			
		value -= d * max;
		max /= 10;
	} while (max != 0);
	
	if (!notZero)
		putc('0');
}

void printHex(u32 value) {

	print("0x");
	
	u32 max = 0x10000000;
	u8 notZero = 0;
	
	do {
		u8 d = value / max;
		notZero |= d;
		
		if (d || notZero) {
			if (d > 9)
				putc('a' + d - 10);
			else
				putc('0' + d);
		}
			
		value -= d * max;
		max /= 0x10;
	} while (max != 0);
	
	if (!notZero)
		putc('0');
}














