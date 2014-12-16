#include <vga.h>
#include <io.h>
#include <types.h>

int stage2(u8 drive) {

	//disable cursor
	out8(0x3d4, 0x0a);
	u8 val = in8(0x3d5);
	val |= 0b00100000;
	out8(0x3d5, val);
	
	clear();
	
	println("booting...");
	
	print("boot drive: ");
	printHex(drive);
	println("");
	
	u8 *boot = (u8 *)0x7c00;
	
	u16 bps = *(u16 *)(boot + 11);
	print("bytes per sector: ");
	printU32(bps);
	println("");
	
	u16 entries = *(u16 *)(boot + 17);
	print("root entries: ");
	printU32(entries);
	println("");
	
	u8 fats = *(boot + 16);
	print("number of FATs: ");
	printU32(fats);
	println("");
	
	u16 spf = *(u16 *)(boot + 22);
	print("sectors per FAT: ");
	printU32(spf);
	println("");
	
	u16 res = *(u16 *)(boot + 14);
	print("reserved sectors: ");
	printU32(res);
	println("");
	
	u32 hidden = *(u32 *)(boot + 28);
	print("hidden sectors: ");
	printU32(hidden);
	println("");
	
	u8 spc = *(boot + 13);
	print("sectors per cluster: ");
	printU32(spc);
	println("");
	
	return 0xbabecafe;
}



















