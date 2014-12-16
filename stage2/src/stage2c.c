#include <vga.h>
#include <io.h>
#include <types.h>

int stage2() {

	//disable cursor
	out8(0x3d4, 0x0a);
	u8 val = in8(0x3d5);
	val |= 0b00100000;
	out8(0x3d5, val);
	
	clear();
	println("stage2...");
	
	return 0xbabecafe;
}