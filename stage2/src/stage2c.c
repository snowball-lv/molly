#include <vga.h>
#include <io.h>
#include <types.h>
#include <disk.h>
#include <stdlib.h>

int stage2(u8 drive) {

	//disable cursor
	out8(0x3d4, 0x0a);
	u8 val = in8(0x3d5);
	val |= 0b00100000;
	out8(0x3d5, val);
	
	clear();
	
	println("booting...");
	print("boot drive: ");
	printHex(drive); println("");
	
	println("VGA signatures:");
	print("    "); println((s8 *)0xc0025);
	print("    "); println((s8 *)0xc007d);
	print("    "); println((s8 *)0xc0008);
	print("    "); println((s8 *)0xc0032);
	
	s8 boot[512];
	read(drive, 0, boot);
	
	u16 rootEntries = *(u16 *)&boot[17];
	print("root entries: ");
	printU32(rootEntries);
	println("");
	
	u16 bps = *(u16 *)&boot[11];
	print("bytes per sector: ");
	printU32(bps);
	println("");
	
	u16 rootSectors = (rootEntries * 32 + 16) / bps;
	print("root sectors: ");
	printU32(rootSectors);
	println("");
	
	u8 FATs = boot[16];
	print("number of FATs: ");
	printU32(FATs);
	println("");
	
	u16 spf = *(u16 *)&boot[22];
	print("sectors per FAT: ");
	printU32(spf);
	println("");
	
	u16 reserved = *(u16 *)&boot[14];
	print("reserved sectors: ");
	printU32(reserved);
	println("");
	
	u16 rootStart = FATs * spf + reserved;
	print("start of root: ");
	printU32(rootStart);
	println("");
	
	u16 data = rootStart + rootSectors;
	print("start of data: ");
	printU32(data);
	println("");
	
	u8 spc = boot[13];
	print("sectors per cluster: ");
	printU32(spc);
	println("");
	
	println("looking for kernel...");
	
	s8 *name = "KERNEL  BIN";
	s8 dir[32];
	
	for (u16 i = 0; i < rootSectors; i++) {
		s8 sec[512];
		read(drive, rootStart + i, sec);
		for (int k = 0; k < 512; k += 32) {
			if (strcmp(name, &sec[k], 11)) {
				copy((u8 *)&sec[k], (u8 *)dir, 32);
			}
		}
	}
	
	if (!strcmp(name, dir, 11)) {
		println("couldn't find kernel.bin");
		return 0xdeadc0de;
	} else {
		println("kernel.bin found");
	}
	
	u16 cluster = *(u16 *)&dir[26];
	print("first cluster: ");
	printU32(cluster);
	println("");
	
	u8 *kernel = (u8 *)0x100000;
	u32 count = 0;
	
	while (1) {
	
		s8 buffer[512];
		for (u8 i = 0; i < spc; i++) {
			u32 sec = (cluster - 2) * spc + data;
			read(drive, sec, buffer);
			copy((u8 *)buffer, kernel, 512);
			kernel += 512;
			count++;
		}
	
		u32 off = (cluster * 2) / 512;
		read(drive, reserved + off, buffer);
		u16 next = *(u16 *)&buffer[cluster * 2];
		
		if (next >= 0xfff8)
			break;
		
		cluster = next;
	}
	
	printU32(count);
	println(" sectors read");
	println("entering kernel");
	
	typedef struct {
		u32 size;
		void *entries;
	} MemMap;
	
	typedef u32 (*kernel_start)(MemMap *);
	kernel_start kstart = (kernel_start)0x100000;
	
	MemMap mm;
	mm.size = *(u32 *)0x900;
	mm.entries = (void *)0x904;
	
	kstart(&mm);
	
	return 0xdeadc0de;
}















