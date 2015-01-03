#include <vmm.h>
#include <types.h>
#include <pmm.h>
#include <console.h>
#include <stdlib.h>
#include <isr.h>

#define PT_INDEX(x)	(((x) >> 12) & 0x3ff)
#define PD_INDEX(x)	(((x) >> 22) & 0x3ff)

static PDirectory *_current_pd;

//vmm_asm.asm
void vmm_load_PDBR(PDirectory *pd);
void vmm_enable_paging();

static void pf_isr(word id, u32 error) {
	printfln("*** page fault!");
	__asm__("hlt");
}

static void vmm_switch_pdir(PDirectory *pd) {
	_current_pd = pd;
	vmm_load_PDBR(_current_pd);
}

#define PAGE_SIZE		4096
#define PAGE_MASK		0xfffff000
#define PRESENT			0b01
#define READ_WRITE		0b10

PTEntry *get_pte(PTable *pt, addr_t addr) {
	return &pt->entries[PT_INDEX(addr)];
}

PTable *get_pt(PDEntry *pde) {
	return (PTable *)(*pde & PAGE_MASK);
}

PDEntry *get_pde(PDirectory *pd, addr_t addr) {
	return &pd->entries[PD_INDEX(addr)];
}

void make_ptable(PDEntry *pde) {
	PTable *pt = pmm_alloc_block();
	memset(pt, 0, sizeof(PTable));
	*pde |= PRESENT;
	*pde |= READ_WRITE;
	*pde |= (addr_t)pt;
	
}

void make_page(PTEntry *pte) {
	void *page = pmm_alloc_block();
	memset(page, 0, PAGE_SIZE);
	*pte |= PRESENT;
	*pte |= READ_WRITE;
	*pte |= (addr_t)page;
}

void map_page(size_t page) {
	addr_t addr = page * PAGE_SIZE;
	
	PDirectory *pd = _current_pd;
	PDEntry *pde = get_pde(pd, addr);
	
	if (!*pde)
		make_ptable(pde);
	
	PTable *pt = get_pt(pde);
	PTEntry *pte = get_pte(pt, addr);
	
	if (!*pte)
		make_page(pte);
	
	*pte |= PRESENT;
	*pte |= READ_WRITE;
	*pte |= addr;
}

void map_pages(size_t first, size_t last) {
	for (size_t i = first; i < last; i++)
		map_page(i);
}

static void map_page_identity(size_t page) {
	addr_t addr = page * PAGE_SIZE;
	
	PDirectory *pd = _current_pd;
	PDEntry *pde = get_pde(pd, addr);
	
	if (!*pde)
		make_ptable(pde);
	
	PTable *pt = get_pt(pde);
	PTEntry *pte = get_pte(pt, addr);
	
	*pte |= PRESENT;
	*pte |= READ_WRITE;
	*pte |= (page * PAGE_SIZE);
}

static void map_pages_identity(size_t first, size_t last) {
	for (size_t i = first; i < last; i++)
		map_page_identity(i);
}

extern void _KERNEL_END;

#define PAGE_INDEX(addr) (addr / PAGE_SIZE)

void initVMM() {

	printfln("sizeof(PTEntry): %d", sizeof(PTEntry));
	printfln("sizeof(PDEntry): %d", sizeof(PDEntry));

	PDirectory *pd = pmm_alloc_block();
	memset(pd, 0, sizeof(PDirectory));
	_current_pd = pd;
	
	addr_t end = (addr_t)&_KERNEL_END;
	size_t last_page = PAGE_INDEX(end);
	
	map_pages_identity(0, last_page);
	
	//set PF isr
	set_isr(14, pf_isr);
	
	vmm_switch_pdir(pd);
	vmm_enable_paging();
}













