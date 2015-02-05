#include <gdt.h>
#include <console.h>
#include <klib.h>
#include <string.h>

#define ATTR __attribute__((packed))

typedef struct {
	uint16_t size;
	uint32_t addr;
} ATTR GDTR;

typedef struct {
	uint16_t 	limit_low;
	uint16_t 	base_low;
	uint8_t 	base_mid;
	uint8_t 	access;
	uint8_t 	limit_high 	: 4;
	uint8_t 	flags 		: 4;
	uint8_t 	base_high;
} ATTR GDTDesc;

#define MAX_DESCRIPTORS 3

static GDTR 	_gdtr;
static GDTDesc 	_gdt[MAX_DESCRIPTORS];

//gdt_asm.asm
void lgdt(GDTR *gdtr);

#define FLAG_32_BIT			0b0100
#define FLAG_4K_GRAN		0b1000

#define ACC_PRESENT			0b10000000

#define ACC_RING_0			0b00000000
#define ACC_RING_1			0b00100000
#define ACC_RING_2			0b01000000
#define ACC_RING_3			0b01100000

#define ACC_SIGN			0b00010000

#define ACC_CODE			0b00001000
#define ACC_DATA			0b00000000

#define ACC_DATA_DIR_UP		0b00000000
#define ACC_DATA_DIR_DOWN	0b00000100

#define ACC_CODE_CONF_ONLY	0b00000000
#define ACC_CODE_CONF_PERM	0b00000100

#define ACC_CODE_READABLE	0b00000010
#define ACC_DATA_WRITABLE	0b00000010


static void gdt_set_gate(	size_t 		num,
							uint32_t 	base,
							uint32_t 	limit,
							uint8_t 	access,
							uint8_t 	flags)
{
	GDTDesc *desc = &_gdt[num];
	
	desc->base_low 		= base & 0xffff;
	desc->base_mid 		= (base >> 16) & 0xff;
	desc->base_high 	= (base >> 24) & 0xff;
	
	desc->limit_low 	= limit & 0xffff;
	desc->limit_high	= (limit >> 16) & 0xf;
	
	desc->access		= access;
	
	desc->flags			= flags & 0xf;
}

void init_gdt() {
	
	ASSERT_SIZE(GDTR, 		6);
	ASSERT_SIZE(GDTDesc, 	8);
	
	//null descriptor
	gdt_set_gate(0, 0, 0, 0, 0);
	
	//flat code descriptor
	gdt_set_gate(
		1,
		0,
		0xfffff,
		0b10011010,
		FLAG_32_BIT | FLAG_4K_GRAN);
	
	//flat data descriptor
	gdt_set_gate(
		2,
		0,
		0xfffff,
		0b10010010,
		FLAG_32_BIT | FLAG_4K_GRAN);
	
	_gdtr.size = sizeof(GDTDesc) * MAX_DESCRIPTORS - 1;
	_gdtr.addr = (uint32_t)&_gdt;
	
	lgdt(&_gdtr);
}
















