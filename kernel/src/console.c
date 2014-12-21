#include <console.h>
#include <types.h>
#include <stdlib.h>

#define VGA_MEM 		0xb8000
#define WIDTH 			80
#define HEIGHT 			25
#define NL 				'\n'
#define VGA_BLACK 		0x0
#define VGA_LIGHT_GRAY 	0x7

static byte attrib = (VGA_BLACK << 4) | VGA_LIGHT_GRAY;
static word xPos = 0;
static word yPos = 0;

void clear() {
	u16 *addr = (u16 *)VGA_MEM;
	u16 blank = attrib << 8 | ' ';
	for (word i = 0; i < WIDTH * HEIGHT; i++)
		*(addr + i) = blank;
	xPos = 0;
	yPos = 0;
}

static void scroll() {
	u16 *dstRow = (u16 *)(VGA_MEM);
	u16 *srcRow = dstRow + WIDTH;
	for (word y = 0; y < HEIGHT - 1; y++) {
		copy(
			(byte *)srcRow,
			(byte *)dstRow,
			WIDTH * 2);
		dstRow = srcRow;
		srcRow = dstRow + WIDTH;
	}
	u16 blank = attrib << 8 | ' ';
	for (word i = 0; i < WIDTH; i++)
		*(dstRow + i) = blank;
}

static void newLine() {
	xPos = 0;
	yPos++;
	if (yPos == HEIGHT) {
		yPos = HEIGHT - 1;
		scroll();
	}
}

static void putc(byte character) {

	if (xPos == WIDTH)
		newLine();
	
	if (character == NL) {
		newLine();
		return;
	}
	
	u16 *addr = (u16 *)(VGA_MEM);
	addr += yPos * WIDTH;
	addr += xPos;
	*addr = attrib << 8 | character;
	xPos++;
}

void print(const byte *str) {
	while (*str != 0)
		putc(*str++);
}

void println(const byte *str) {
	print(str);
	putc(NL);
}

//don't touch
static byte *itoa(word value, byte *str, word base) {
    byte *rc;
    byte *ptr;
    byte *low;
    // Check for supported base.
    if (base < 2 || base > 36) {
		*str = null;
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    // Terminating the string.
    *ptr-- = null;
    // Invert the numbers.
    while (low < ptr) {
        byte tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

void printWord(word value) {
	byte buffer[WORD_BITS + 1];
	byte *str = itoa(value, buffer, 10);
	print(str);
}











