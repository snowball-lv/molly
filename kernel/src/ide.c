#include <ide.h>
#include <console.h>

void init_ide(int r0, int r1, int r2, int r3, int r4) {
	
	kprintfln("init ide");
	
	kprintfln(
		"%x, %x, %x, %x, %x",
		r0, r1, r2, r3, r4);
		
	r0 = (r0 == 0x0 || r0 == 0x1) ? 0x1f0 : r0;
	r1 = (r1 == 0x0 || r1 == 0x1) ? 0x3f6 : r1;
	r2 = (r2 == 0x0 || r2 == 0x1) ? 0x170 : r2;
	r3 = (r3 == 0x0 || r3 == 0x1) ? 0x376 : r3;
	
	kprintfln(
		"%x, %x, %x, %x, %x",
		r0, r1, r2, r3, r4);
		
}