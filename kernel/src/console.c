#include <console.h>
#include <klib.h>
#include <string.h>
#include <stdarg.h>
#include <param.h>
#include <devfs.h>
#include <vfs.h>
#include <kalloc.h>
#include <debug.h>
#include <scancodes.h>
#include <pipe.h>
#include <ringbuffer.h>

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

static int console_open(vnode *vn);
static int console_close(fs_node *fn);
static int console_write(fs_node *fn, void *buff, size_t off, int count);
static int console_read(fs_node *fn, void *buff, size_t off, int count);

static vnode console = {

	.open = console_open,
	.close = console_close,
	.write = console_write,
	.read = console_read

};

typedef struct {

	vnode 	*vga;
	vnode 	*kbd;

} console_node;

static ringbuffer_t *rb;

static int console_open(vnode *vn) {
	logfln("console open");

	rb = ringbuffer_create(64);

	vnode *vga = vfs_open("#vga");

	if (vga == 0) {
		logfln("vga device not found");
		return -1;
	}

	vnode *kbd = vfs_open("#kbd");

	if (kbd == 0) {
		logfln("kbd device not found");
		return -1;
	}

	console_node *cn = kmalloc(sizeof(console_node));

	cn->vga = vga;
	cn->kbd = kbd;

	vn->fn = cn;

	return 1;
}

static int console_close(fs_node *fn) {
	logfln("console close");

	console_node *cn = (console_node *)fn;

	int rc0 = vfs_close(cn->vga);
	int rc1 = vfs_close(cn->kbd);

	kfree(cn);

	return (rc0 == 0 && rc1 == 0) ? 0 : -1;
}

static int console_write(fs_node *fn, void *buff, size_t off, int count) {
	kprintfln("console write");

	console_node *cn = (console_node *)fn;
	vnode *vga = cn->vga;

	return vfs_write(vga, buff, off, count);
}

static int console_read(fs_node *fn, void *buff, size_t off, int count) {
	logfln("console read");

	if (count == 0)
		return 0;

	console_node *cn = (console_node *)fn;
	vnode *kbd = cn->kbd;

	key_event e;

	while (1) {

		kbd->read_event(kbd->fn, &e);
		char a = get_ascii(e.key);

		if (a == 0)
			continue;

		size_t space = ringbuffer_space(rb);

		if (space == 1 && e.key != KEY_ENTER)
			continue;

		kprintf("%c", a);
		ringbuffer_write(rb, (char[]){ a }, 1);

		if (e.key == KEY_ENTER) {
			size_t unread = ringbuffer_unread(rb);
			size_t good = (count < unread) ? count : unread;
			size_t read = ringbuffer_read(rb, buff, good);
			return read;
		}

	}

	return 0;
}

static int vga_open(vnode *vn);
static int vga_close(fs_node *fn);
static int vga_write(fs_node *fn, void *buff, size_t off, int count);

static vnode vga = {

	.open 	= vga_open,
	.close 	= vga_close,
	.write 	= vga_write

};

static int vga_open(vnode *vn) {
	logfln("vga open");
	return 1;
}

static int vga_close(fs_node *fn) {
	logfln("vga close");
	return 0;
}

static int vga_write(fs_node *fn, void *buff, size_t off, int count) {
	logfln("vga write");

	uint8_t *cbuff = buff;
	for (int i = 0; i < count; i++)
		logfln("%x", cbuff[i]);

	return count;
}

void init_console() {
	kprintfln("init console");

	if (dev_add("vga", &vga) == -1)
		panic("*** couldn't add vga device");

	if (dev_add("console", &console) == -1)
		panic("*** couldn't add console device");
}














