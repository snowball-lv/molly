#include <paging.h>

#include <pmm.h>
#include <console.h>
#include <klib.h>
#include <interrupt.h>
#include <types.h>
#include <stdint.h>
#include <proc.h>

#define PAGE_PRESENT		(0b01)
#define PAGE_READ_WRITE		(0b10)

void map_pde(pd_t *pd, size_t index, void *phys) {

	pde_t *pde = &pd->entries[index];
	
	*pde |= PAGE_PRESENT;
	*pde |= PAGE_READ_WRITE;
	*pde |= (uintptr_t)phys;
}

void map_pte(pt_t *pt, size_t index, void *phys) {

	pte_t *pte = &pt->entries[index];
	
	*pte |= PAGE_PRESENT;
	*pte |= PAGE_READ_WRITE;
	*pte |= (uintptr_t)phys;
}

static void alloc_page(size_t index) {

	pd_t *pd = (pd_t *)0xfffff000;
	pde_t *pde = &pd->entries[PAGE_PDE(index)];
	
	if (*pde == 0) {
		kprintfln("allocating new page table");
		void *page_table = pmm_alloc_block();
		memset(page_table, 0, PAGE_SIZE);
		map_pde(pd, PAGE_PDE(index), page_table);
	}
	
	pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PAGE_PDE(index));
	pte_t *pte = &pt->entries[PAGE_PTE(index)];
	
	if (*pte == 0) {
		kprintfln("allocating page: %d", index);
		void *phys = pmm_alloc_block();
		kprintfln("phys: %x", phys);
		map_pte(pt, PAGE_PTE(index), phys);
	}
}

void alloc_pages(size_t first, size_t count) {
	for (size_t i = first; i < first + count; i++)
		alloc_page(i);
}

static int is_pt_empty(pt_t *pt) {
	for (size_t i = 0; i < 1024; i++)
		if (pt->entries[i] != 0)
			return 0;
	return 1;
}

static void free_page(size_t index) {

	//delete empty page
	pd_t *pd = (pd_t *)0xfffff000;
	pde_t *pde = &pd->entries[PAGE_PDE(index)];
	
	if (*pde == 0)
		return;
		
	pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PAGE_PDE(index));
	pte_t *pte = &pt->entries[PAGE_PTE(index)];
	
	if (*pte != 0) {
		pmm_free_block((void *)(*pte & PAGE_MASK));
		*pte = 0;
	}
	
	//delete empty page table
	if (is_pt_empty(pt)) {
		pmm_free_block((void *)(*pde & PAGE_MASK));
		*pde = 0;
	}
}

void free_pages(size_t first, size_t count) {
	for (size_t i = first; i < first + count; i++)
		free_page(i);
}

static void pf_isr(trapframe_t *tf) {
	kprintfln("*** page fault! %x", tf->err_code);
	stop();
}

//paging_asm.asm
void load_PDBR(pd_t *pd);
void enable_paging();

void init_paging() {
	
	ASSERT_SIZE(pte_t, 4);
	ASSERT_SIZE(pde_t, 4);
	
	proc_t *p = get_null_proc();
	
	load_PDBR(p->pd);
	
	enable_paging();
	
	//set PF isr
	isr_set_handler(14, pf_isr);
}

#define ADDR_PDE(addr)	(PAGE_PDE((uintptr_t)(addr) / PAGE_SIZE))
#define ADDR_PTE(addr)	(PAGE_PTE((uintptr_t)(addr) / PAGE_SIZE))

static pt_t *get_pt(size_t index) {
	return (pt_t *)(0xffc00000 + PAGE_SIZE * index);
}

void *virt_to_phys(void *virt) {
	
	pt_t *pt = get_pt(ADDR_PDE(virt));
	pte_t *pte = &pt->entries[ADDR_PTE(virt)];
	return (void *)(*pte & PAGE_MASK);
}
















