#include <console.h>
#include <klib.h>
#include <string.h>
#include <stdarg.h>
#include <param.h>
#include <devfs.h>
#include <vfs.h>

#define VGA_MEM 		(0xb8000 + KERNEL_OFF)
#define WIDTH 			(80)
#define HEIGHT 			(25)
#define VGA_BLACK 		(0x0)
#define VGA_LIGHT_GRAY 	(0x7)

static char attrib = (VGA_BLACK << 4) | VGA_LIGHT_GRAY;
static int xPos = 0;
static int yPos = 0;

void console_clear() {

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
static char *itoa2(int value, char *str, int base) {
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

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char *itoa(int value, char *result, int base) {

	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	
	return result;
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
	
	va_list args;
    va_start(args, format);
	
	const char *str = format;
	
	for (size_t i = 0; str[i] != 0; i++) {
		if (str[i] == '%') {
		
			switch (str[i + 1]) {
			case 'd':
				printDec(va_arg(args, int));
				i++;
				break;
			case 'c':
				putc(va_arg(args, int));
				i++;
				break;
			case 'x':
				printHex(va_arg(args, int));
				i++;
				break;
			case 'b':
				printBin(va_arg(args, int));
				i++;
				break;
			case 's':
				puts(va_arg(args, char *));
				i++;
				break;
			default:
				putc(str[i]);
				break;
			}
			
		} else {
			putc(str[i]);
		}
	}
	
	va_end(args);
}

static int console_open(vnode *vn, char *path) {
	kprintfln("console open: [%s]", path);
	return 0;
}

static vnode console = {

	.open = console_open

};

void init_console() {
	kprintfln("inti console");
	dev_add("console", &console);
}















