#include <vmm.h>
#include <types.h>
#include <pmm.h>
#include <console.h>
#include <stdlib.h>
#include <isr.h>

static PDirectory *_dir;

PDirectory *vmm_get_dir() {
	return _dir;
}

//vmm_asm.asm
void vmm_load_PDBR(PDirectory *dir);
void vmm_enable_paging();

static void pf_isr(word id, u32 error) {
	printfln("*** page fault!");
	__asm__("hlt");
}

static void vmm_switch_pdir(PDirectory *dir) {
	_dir = dir;
	vmm_load_PDBR(_dir);
}

extern void _KERNEL_END;

void initVMM() {

	printfln("sizeof(PTEntry): %d", sizeof(PTEntry));
	printfln("sizeof(PDEntry): %d", sizeof(PDEntry));

	addr_t end = (addr_t)&_KERNEL_END;
	size_t pages = end / PMM_BLOCK_SIZE;
	size_t tables = (pages + 1023) / 1024;
	
	PDirectory *dir = pmm_alloc_block();
	memset(dir, 0, sizeof(PDirectory));
	
	size_t pi = 0;
	for (size_t t = 0; t < tables; t++) {
	
		PTable *pt = pmm_alloc_block();
		memset(pt, 0, sizeof(PTable));
		
		for (; pi < pages; pi++) {
			
			PTEntry pe = 0;
			pe |= 0b1;			//present
			pe |= 0b10;			//read/write
			pe |= pi * 4096;	//address
			pt->entries[pi % 1024] = pe;
			
		}
		
		PDEntry de = 0;
		de |= 0b1;			//present
		de |= 0b10;			//read/write
		de |= (addr_t)pt;	//address
		dir->entries[t] = de;
		
	}
	
	//set PF isr
	set_isr(14, pf_isr);
	
	vmm_switch_pdir(dir);
	vmm_enable_paging();
}

void vmm_alloc_page(PTEntry *p) {
	void *b = pmm_alloc_block();
	*p |= 0b1;			//present
	*p |= 0b10;			//read/write
	*p |= (addr_t)b;	//address
}

void vmm_free_page(PTEntry *p) {
	addr_t addr = *p & 0xfffff000;
	pmm_free_block((void *)addr);
	*p = 0;
}

PTEntry *vmm_get_ptentry(PTable *t, addr_t addr) {
	return &t->entries[PT_INDEX(addr)];
}

PDEntry *vmm_get_pdentry(PDirectory *d, addr_t addr) {
	return &d->entries[PD_INDEX(addr)];
}













