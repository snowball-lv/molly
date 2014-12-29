#include <vmm.h>
#include <types.h>
#include <pmm.h>
#include <console.h>
#include <stdlib.h>
#include <idt.h>

#define PT_INDEX(x)	(((x) >> 12) & 0x3ff)
#define PD_INDEX(x)	(((x) >> 22) & 0x3ff)

typedef u32 PTEntry;

typedef struct {
	PTEntry entries[1024];
} PTable;

typedef u32 PDEntry;

typedef struct {
	PDEntry entries[1024];
} PDirectory;

static PDirectory *_dir;

//vmm_asm.asm
void vmm_load_PDBR(PDirectory *dir);
void vmm_enable_paging();

static void pf_isr() {
	printfln("*** page fault!");
}

static void vmm_switch_pdir(PDirectory *dir) {
	_dir = dir;
	vmm_load_PDBR(_dir);
}

void initVMM() {

	printfln("sizeof(PTEntry): %d", sizeof(PTEntry));
	printfln("sizeof(PDEntry): %d", sizeof(PDEntry));

	//identity
	PTable *ident = pmm_alloc_block();
	memset(ident, 0, sizeof(PTable));
	for (word i = 0; i < 1024; i++) {
		PTEntry e = 0;
		e |= 0b1;		//present
		e |= 0b10;		//read/write
		e |= i * 4096;	//address
		ident->entries[i] = e;
	}
	
	//3gb
	PTable *high = pmm_alloc_block();
	memset(high, 0, sizeof(PTable));
	uword frame = 0x100000;
	for (word i = 0; i < 1024; i++) {
		PTEntry e = 0;
		e |= 0b1;		//present
		e |= 0b10;		//read/write
		e |= frame;		//address
		high->entries[i] = e;
		frame += 4096;
	}
	
	PDirectory *dir = pmm_alloc_block();
	memset(dir, 0, sizeof(PDirectory));
	
	//identity
	PDEntry e = 0;
	e |= 0b1;			//present
	e |= 0b10;			//read/write
	e |= (uword)ident;	//address
	dir->entries[PD_INDEX(0x0)] = e;
	
	//3gb
	PDEntry e2 = 0;
	e2 |= 0b1;			//present
	e2 |= 0b10;			//read/write
	e2 |= (uword)high;	//address
	dir->entries[PD_INDEX(0xc0000000)] = e2;
	
	//set PF isr
	set_gate(14, (u32)&pf_isr);
	
	vmm_switch_pdir(dir);
	
	vmm_enable_paging();
}












