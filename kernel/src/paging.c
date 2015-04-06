#include <paging.h>
#include <console.h>
#include <pmm.h>
#include <kalloc.h>
#include <proc.h>

//paging_asm.asm
void load_PDBR(pd_t *pd);
void enable_paging();
	
#define PDEI(a)		(((uintptr_t)(a) >> 22) & 0x3ff)
#define PTEI(a)		(((uintptr_t)(a) >> 12) & 0x3ff)

#define _4KB_MASK(v)	((v) & 0xfffff000)
	
void map_page(void *virt, void *phys, int flags) {

	pd_t *pd = (pd_t *)0xfffff000;
	pde_t *pde = &pd->entries[PDEI(virt)];
	
	if (*pde == 0) {
		kprintfln("allocating new page table");
		void *table = pmm_alloc_block();
		*pde = PTE_P | PTE_RW | PTE_U | (uintptr_t)table;
		pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PDEI(virt));
		memset(pt, 0, PAGE_SIZE);
	}

	pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PDEI(virt));
	pte_t *pte = &pt->entries[PTEI(virt)];
	
	if (*pte != 0) {
		kprintfln("remapping! deleting old frame.");
		pmm_free_block((void *)_4KB_MASK(*pte));
	}
	
	*pte = (uintptr_t)phys | flags;
}

#define _4MB_MASK(v)	((v) & 0xffc00000)
#define _4MB_OFF(v)		((v) & 0x003fffff)
#define _4KB_OFF(v)		((v) & 0x00000fff)

void *vtp(void *virt) {
	
	pd_t *pd = (pd_t *)0xfffff000;
	pde_t *pde = &pd->entries[PDEI(virt)];
	
	if (*pde & PTE_PS) {
		char *page = (char *)_4MB_MASK(*pde);
		return page + _4MB_OFF((uintptr_t)virt);
	}
	
	pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PDEI(virt));
	pte_t *pte = &pt->entries[PTEI(virt)];
	
	char *page = (char *)_4KB_MASK(*pte);
	return page + _4KB_OFF((uintptr_t)virt);
}

pd_t *clone_pd() {
	
	pd_t *cpd = (pd_t *)0xfffff000;
	
	return cpd;
}

















