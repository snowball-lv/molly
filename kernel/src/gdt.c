#include <gdt.h>
#include <types.h>
#include <console.h>
#include <stdlib.h>

#define MAX_DESCRIPTORS 3
#define ATTR __attribute__((packed))

typedef struct {
	u16 size;
	u32 addr;
} ATTR GDTR;

typedef struct {
	u16 limitLow;
	u16 baseLow;
	u8 baseMid;
	u8 access;
	u8 limitHigh : 4;
	u8 flags : 4;
	u8 baseHigh;
} ATTR GDTDesc;

static GDTR 	_gdtr;
static GDTDesc 	_gdt[MAX_DESCRIPTORS];

//gdt_asm.asm
void lgdt(GDTR *gdtr);

void initGDT() {
	printfln("MAX_DESCRIPTORS: %d", MAX_DESCRIPTORS);
	printfln("sizeof(GDTR): %d", sizeof(GDTR));
	printfln("sizeof(GDTDesc): %d", sizeof(GDTDesc));
	
	//null descriptor
	memset((byte *)&_gdt[0], 0, sizeof(GDTDesc));
	
	//code descriptor
	_gdt[1].limitLow = 0xffff;
	_gdt[1].baseLow = 0;
	_gdt[1].baseMid = 0;
	_gdt[1].access = 0b10011010;
	_gdt[1].limitHigh = 0xf;
	_gdt[1].flags = 0b1100;
	_gdt[1].baseHigh = 0;
	
	//data descriptor
	_gdt[2].limitLow = 0xffff;
	_gdt[2].baseLow = 0;
	_gdt[2].baseMid = 0;
	_gdt[2].access = 0b10010010;
	_gdt[2].limitHigh = 0xf;
	_gdt[2].flags = 0b1100;
	_gdt[2].baseHigh = 0;
	
	_gdtr.size = sizeof(GDTDesc) * MAX_DESCRIPTORS - 1;
	_gdtr.addr = (u32)&_gdt;
	
	lgdt(&_gdtr);
}
















