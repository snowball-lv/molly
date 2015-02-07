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
		*pde |= VMM_PRESENT;
		*pde |= VMM_READ_WRITE;
		*pde |= (uintptr_t)pmm_alloc_block();
	}
	
	pt_t *pt = (pt_t *)(0xffc00000 + PAGE_SIZE * PD_INDEX(addr));
	pte_t *pte = vmm_get_pte(pt, addr);
	
	if (*pte == 0) {
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

typedef struct header_t header_t;

struct header_t {
	int 		state;
	size_t 		size;
	header_t	*next;
	void		*mem;
};

static header_t *first() {
	proc_t *p = get_kproc();
	if (p->heap_base == p->heap_top)
		return 0;
	return p->heap_base;
}

static header_t *next(header_t *h) {
	return h == 0 ? first() : h->next;
}

#define ALIGN_RIGHT(v, a)	\
	(((v) + (a) - 1) & ~((a) - 1))

static void *kmalloc_custom(size_t size, size_t alignment) {

	kprintfln("kmalloc %d, %d", size, alignment);
	
	if (size == 0)
		return 0;
	
	//check if alignment is natural
	ASSERT_ALIGNMENT(alignment, MEM_ALIGNMENT);

	header_t *h = 0;
	
	while ((h = next(h)) != 0) {
		//recycle usable chunks
	}
	
	//no reusable chunks, allocate more
	
	kprintfln("no reusable chunks");
	
	//should be page aligned
	void *brk = get_kproc()->heap_top;
	
	ASSERT_PAGE_ALIGNED((uintptr_t)brk);
	
	void *new_brk = (char *)brk + sizeof(header_t);
	
	void *mem_start = (void *)
		ALIGN_RIGHT((uintptr_t)new_brk, alignment);
		
	new_brk = (char *)mem_start + size;
	
	ASSERT_ALIGNMENT((uintptr_t)new_brk, MEM_ALIGNMENT);
	
	kprintfln("old brk: %d", brk);
	kprintfln("new brk: %d", new_brk);

	return 0;
}

void *kmalloc_page() {
	return kmalloc_custom(PAGE_SIZE, PAGE_SIZE);
}

void *kmalloc(size_t size) {
	return kmalloc_custom(size, MEM_ALIGNMENT);
}




















