#include <console.h>
#include <klib.h>
#include <string.h>
#include <stdarg.h>
#include <sync.h>

#define VGA_MEM 		0xb8000
#define WIDTH 			80
#define HEIGHT 			25
#define VGA_BLACK 		0x0
#define VGA_LIGHT_GRAY 	0x7

static char attrib = (VGA_BLACK << 4) | VGA_LIGHT_GRAY;
static int xPos = 0;
static int yPos = 0;

static mutex_t _m;

void console_clear() {

	mutex_init(&_m);

	uint16_t *addr = (uint16_t *)VGA_MEM;
	uint16_t blank = attrib << 8 | ' ';
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		*(addr + i) = blank;
	xPos = 0;
	yPos = 0;
}

static void scroll() {
	uint16_t *dstRow = (uint16_t *)(VGA_MEM);
	uint16_t *srcRow = dstRow + WIDTH;
	for (int y = 0; y < HEIGHT - 1; y++) {
		memcpy(dstRow, srcRow, WIDTH * 2);
		dstRow = srcRow;
		srcRow = dstRow + WIDTH;
	}
	uint16_t blank = attrib << 8 | ' ';
	for (int i = 0; i < WIDTH; i++)
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

static void putc(char character) {

	if (xPos >= WIDTH)
		newLine();
	
	if (character == LF) {
		newLine();
		return;
	} else if (character == CR) {
		xPos = 0;
		return;
	} else if (character == TAB) {
		xPos = (xPos + 3) & (~4);
		return;
	}
	
	uint16_t *addr = (uint16_t *)(VGA_MEM);
	addr += yPos * WIDTH;
	addr += xPos;
	*addr = attrib << 8 | character;
	xPos++;
}

static void puts(const char *str) {
	while (*str != 0)
		putc(*str++);
}

//don't touch
static char *itoa(int value, char *str, int base) {
    char *rc;
    char *ptr;
    char *low;
    // Check for supported base.
    if (base < 2 || base > 36) {
		*str = 0;
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
    *ptr-- = 0;
    // Invert the numbers.
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

static void printDec(int value) {
	char buffer[(sizeof(int) * 8) + 1];
	char *str = itoa(value, buffer, 10);
	puts(str);
}

static void printHex(int value) {
	char buffer[(sizeof(int) * 8) + 1];
	char *str = itoa(value, buffer, 16);
	puts("0x");
	puts(str);
}

static void printBin(int value) {
	char buffer[(sizeof(int) * 8) + 1];
	char *str = itoa(value, buffer, 2);
	puts(str);
	puts("b");
}

void kprintf(const char *format, ...) {

	mutex_lock(&_m);

	va_list args;
    va_start(args, format);
	
	while (*format != 0) {
		switch (*format) {
		case '%': 
			switch (*(format + 1)) {
			case 'd':
				printDec(va_arg(args, int));
				format += 2;
			break;
			case 'x':
				printHex(va_arg(args, int));
				format += 2;
			break;
			case 'b':
				printBin(va_arg(args, int));
				format += 2;
			break;
			case 'c':
				putc(va_arg(args, int));
				format += 2;
			break;
			case 's':
				puts(va_arg(args, char *));
				format += 2;
			break;
			default: putc(*format++); break;
			}
		break;
		default: putc(*format++); break;
		}
	}
	
	va_end(args);
	
	mutex_release(&_m);
}










