#include <disk.h>

u32 read(u8 drive, u32 sector, s8 *buffer) {
	
	u32 addr = (u32)buffer;
	u32 seg = addr - 0x10000; 
	u32 off = 0x10 - (seg & 0xf);
	seg = (seg + off) >> 4;
	off = 0x10000 - off;
	
	typedef u32 (*Int13Ptr)(
		u8 drive,
		u32 sector,
		u16 seg,
		u16 off);
		
	Int13Ptr int13 = (Int13Ptr)0x700;
	return int13(drive, sector, seg, off);
}








