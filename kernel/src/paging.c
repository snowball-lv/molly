#include <paging.h>
#include <console.h>
#include <pmm.h>
#include <kalloc.h>
#include <proc.h>
#include <molly.h>
	
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
	
	*pte = (uintptr_t)phys | flags;
	
	invlpg(virt);
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

static void map_page2(void *virt, void *phys, int flags) {

	pd_t *pd = (pd_t *)0xffffe000;
	pde_t *pde = &pd->entries[PDEI(virt)];
	
	if (*pde == 0) {
		kprintfln("allocating new page table - 2");
		void *table = pmm_alloc_block();
		*pde = PTE_P | PTE_RW | PTE_U | (uintptr_t)table;
		pt_t *pt = (pt_t *)(0xff800000 + PAGE_SIZE * PDEI(virt));
		memset(pt, 0, PAGE_SIZE);
	}

	pt_t *pt = (pt_t *)(0xff800000 + PAGE_SIZE * PDEI(virt));
	pte_t *pte = &pt->entries[PTEI(virt)];
	
	*pte = (uintptr_t)phys | flags;
}

pd_t *clone_pd() {
	
	pd_t *npd = kmalloc_page();
	memset(npd, 0, PAGE_SIZE);
	
	pd_t *cpd = (pd_t *)0xfffff000;
	
	for (int i = 768; i < 1024; i++)
		npd->entries[i] = cpd->entries[i];
		
	pde_t *rec = &npd->entries[1023];
	*rec = (uintptr_t)vtp(npd) | PTE_P | PTE_RW;
		
	pde_t *mp2 = &cpd->entries[1022];
	*mp2 = (uintptr_t)vtp(npd) | PTE_P | PTE_RW;
	
	reloadPDBR();
	
	char *virt 	= (char *)0x0;
	char *brk 	= cproc()->brk;
	char *buff	= kmalloc_page();
	
	kprintfln("brk: %x ", brk);
	kprintfln("pages: %d ", (uintptr_t)brk / PAGE_SIZE);
	
	for (;virt < brk; virt += PAGE_SIZE) {
		void *phys = pmm_alloc_block();
		map_page(buff, phys, PTE_P | PTE_RW | PTE_U);
		memcpy(buff, virt, PAGE_SIZE);
		map_page2(virt, phys, PTE_P | PTE_RW | PTE_U);
	}
	
	return npd;
}

pd_t *create_pd() {

	pd_t *pd = kmalloc_page();
	memset(pd, 0, PAGE_SIZE);
	
	pd_t *cpd = (pd_t *)0xfffff000;
	
	//copy kernel pages
	for (int i = 768; i < 1024; i++)
		pd->entries[i] = cpd->entries[i];
		
	//add last PT recursion
	pde_t *rec = &pd->entries[1023];
	*rec = (uintptr_t)vtp(pd) | PTE_P | PTE_RW;
	
	return pd;
}














