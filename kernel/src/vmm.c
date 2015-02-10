#include <vmm.h>
#include <pmm.h>
#include <console.h>
#include <klib.h>
#include <interrupt.h>
#include <types.h>
#include <proc.h>
#include <stdint.h>

//vmm_asm.asm
void vmm_load_PDBR(pd_t *pd);
void vmm_enable_paging();

#define PT_INDEX(x)	(((x) >> 12) & 0x3ff)
#define PD_INDEX(x)	(((x) >> 22) & 0x3ff)

pte_t *vmm_get_pte(pt_t *pt, uintptr_t addr) {
	return &pt->entries[PT_INDEX(addr)];
}

pde_t *vmm_get_pde(pd_t *pd, uintptr_t addr) {
	return &pd->entries[PD_INDEX(addr)];
}

void vmm_alloc_page(size_t num) {

	pd_t *pd = (pd_t *)0xfffff000;
	uintptr_t addr = num * PAGE_SIZE;
	pde_t *pde = vmm_get_pde(pd, addr);
	
	if (*pde == 0) {
		kprintfln("allocating new page table");
		*pde |= VMM_PRESENT;
		*pde |= VMM_READ_WRITE;
		*pde |= (uintptr_t)pmm_alloc_block();
	}
	
	pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PD_INDEX(addr));
	pte_t *pte = vmm_get_pte(pt, addr);
	
	if (*pte == 0) {
		kprintfln("allocating page: %d", num);
		*pte |= VMM_PRESENT;
		*pte |= VMM_READ_WRITE;
		*pte |= (uintptr_t)pmm_alloc_block();
	}
}

void vmm_alloc_pages(size_t first, size_t count) {
	for (size_t i = first; i < first + count; i++)
		vmm_alloc_page(i);
}

static int is_pt_empty(pt_t *pt) {
	for (size_t i = 0; i < 1024; i++)
		if (pt->entries[i] != 0)
			return 0;
	return 1;
}

static void vmm_free_page(size_t num) {

	//delete empty page
	pd_t *pd = (pd_t *)0xfffff000;
	uintptr_t addr = num * PAGE_SIZE;
	pde_t *pde = vmm_get_pde(pd, addr);
	
	if (*pde == 0)
		return;
		
	pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PD_INDEX(addr));
	pte_t *pte = vmm_get_pte(pt, addr);
	
	if (*pte != 0) {
		pmm_free_block((void *)(*pte & VMM_PAGE_MASK));
		*pte = 0;
	}
	
	//delete empty page table
	if (is_pt_empty(pt)) {
		pmm_free_block((void *)(*pde & VMM_PAGE_MASK));
		*pde = 0;
	}
}

void vmm_free_pages(size_t first, size_t count) {
	for (size_t i = first; i < first + count; i++)
		vmm_free_page(i);
}

static void pf_isr(trapframe_t *tf) {
	kprintfln("*** page fault! %x", tf->err_code);
	stop();
}

void init_vmm() {
	
	ASSERT_SIZE(pte_t, 4);
	ASSERT_SIZE(pde_t, 4);
	
	proc_t *kproc = get_kproc();
	
	vmm_load_PDBR(kproc->pd);
	vmm_enable_paging();
	
	//set PF isr
	isr_set_handler(14, pf_isr);
}

#define MEM_USED		(1)
#define MEM_FREE		(2)

static void *kmalloc_custom(size_t size, size_t alignment) {

	//enter critical section
	disable_ints();
	
	kprintfln("kmalloc: %d, %d", size, alignment);
	
	if (size == 0)
		return 0;
		
	proc_t *p = get_kproc();
	uintptr_t brk = (uintptr_t)p->heap_top;
	
	size_t first = PAGE_ALIGN_RIGHT(brk) / PAGE_SIZE;
	
	kprintfln("first: %d", first);
	
	uintptr_t mem = ALIGN_RIGHT(brk, alignment);
	uintptr_t new_brk = mem + size;
	new_brk = MEM_ALIGN_RIGHT(new_brk);
	
	size_t old_page = PAGE_ALIGN_RIGHT(brk) / PAGE_SIZE;
	size_t new_page = PAGE_ALIGN_RIGHT(new_brk) / PAGE_SIZE;
	size_t count = new_page - old_page;
	
	kprintfln("count: %d", count);
	
	vmm_alloc_pages(first, count);
	
	p->heap_top = (void *)new_brk;
	
	//exit critical section
	enable_ints();
	
	return (void *)mem;
}

void *kmalloc_page() {
	return kmalloc_custom(PAGE_SIZE, PAGE_SIZE);
}

void *kmalloc(size_t size) {
	return kmalloc_custom(size, MEM_ALIGNMENT);
}

void kfree(void *ptr) {
	
}


















