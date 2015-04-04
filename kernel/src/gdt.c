#include <gdt.h>
#include <console.h>
#include <stdint.h>

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

_Static_assert(sizeof(GDTR) 	== 6, "GDTR size not 6");
_Static_assert(sizeof(GDTDesc)	== 8, "GDTDesc size not 8");

//A struct describing a Task State Segment.
typedef struct {

	// The previous TSS - if we used hardware task switching this would form a linked list.
	uint32_t prev_tss;
	
	//The stack pointer to load when we change to kernel mode.
	uint32_t esp0;
	
	//The stack segment to load when we change to kernel mode.
	uint32_t ss0;
	
	//everything below here is unusued now.. 
	
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;         
	uint32_t cs;        
	uint32_t ss;        
	uint32_t ds;        
	uint32_t fs;       
	uint32_t gs;         
	uint32_t ldt;      
	uint16_t trap;
	uint16_t iomap_base;
	
} ATTR tss_entry_t;

_Static_assert(
	sizeof(tss_entry_t) == 104,
	"tss_entry_t size not 104");

#define F_32_BIT		(0b0100)
#define F_4K_GRAN		(0b1000)

//gdt_asm.asm
void lgdt(GDTR *gdtr);

#define MAX_DESCRIPTORS		(6)

static tss_entry_t tss_entry;

static GDTDesc gdt[MAX_DESCRIPTORS] = {
	{
		//null descriptor
		.base_low 	= 0,
		.base_mid	= 0,
		.base_high 	= 0,
		.limit_low 	= 0,
		.limit_high = 0,
		.access		= 0,
		.flags		= 0
	}, {
		//flat kernel code descriptor
		.base_low 	= 0,
		.base_mid	= 0,
		.base_high 	= 0,
		.limit_low 	= 0xffff,
		.limit_high = 0xf,
		.access		= 0b10011010,
		.flags		= F_32_BIT | F_4K_GRAN
	}, {
		//flat kernel data descriptor
		.base_low 	= 0,
		.base_mid	= 0,
		.base_high 	= 0,
		.limit_low 	= 0xffff,
		.limit_high = 0xf,
		.access		= 0b10010010,
		.flags		= F_32_BIT | F_4K_GRAN
	}, {
		//flat user code descriptor
		.base_low 	= 0,
		.base_mid	= 0,
		.base_high 	= 0,
		.limit_low 	= 0xffff,
		.limit_high = 0xf,
		.access		= 0b11111010,
		.flags		= F_32_BIT | F_4K_GRAN
	}, {
		//flat user data descriptor
		.base_low 	= 0,
		.base_mid	= 0,
		.base_high 	= 0,
		.limit_low 	= 0xffff,
		.limit_high = 0xf,
		.access		= 0b11110010,
		.flags		= F_32_BIT | F_4K_GRAN
	}, {
		//tss (set up in init)
		.base_low 	= 0,
		.base_mid	= 0,
		.base_high 	= 0,
		.limit_low 	= 0,
		.limit_high = 0,
		.access		= 0,
		.flags		= 0
	}
};

static GDTR gdtr = {
	.size = sizeof(GDTDesc) * MAX_DESCRIPTORS - 1,
	.addr = (uint32_t)&gdt
};

//gdt_asm.asm
void tss_flush();

void set_tss(void *esp0) {
	tss_entry.esp0 = (uintptr_t)esp0;
}

void init_gdt() {
	
	lgdt(&gdtr);
	
	uintptr_t base = (uintptr_t)&tss_entry;
	uintptr_t limit = base + sizeof(tss_entry_t);
	
	GDTDesc *tssd = &gdt[5];
	
	tssd->base_low 	= base & 0xffff;
	tssd->base_mid 	= (base >> 16) & 0xff;
	tssd->base_high = (base >> 24) & 0xff;
	
	tssd->limit_low 	= limit & 0xffff;
	tssd->limit_high 	= (limit >> 16) & 0xf;
	
	tssd->access 	= 0b11101001;
	tssd->flags 	= 0b0000;
	
	tss_entry.ss0 = 0x10;
	
	tss_flush();
}














