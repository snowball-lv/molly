#include <proc.h>

#include <paging.h>
#include <pmm.h>
#include <types.h>

static proc_t _null_proc;

proc_t *get_null_proc() {
	return &_null_proc;
}

extern none_t _KERNEL_END;

void init_null_proc() {
	
	//allocate page directory
	_null_proc.pd = pmm_alloc_block();
	pd_t *pd = _null_proc.pd;
	memset(pd, 0, sizeof(pd_t));
	
	//allocate empty page tables for kernel space (1 GB)
	for (int i = 0; i < 256; i++) {
		
		void *page_table = pmm_alloc_block();
		memset(page_table, 0, PAGE_SIZE);
		map_pde(pd, i, page_table);
	}
	
	//recursive map last pde
	map_pde(pd, 1023, pd);
	
	//page count to identity map
	size_t count = (uintptr_t)&_KERNEL_END / PAGE_SIZE;
	
	//identity map kernel pages
	//omit first page to make 0 pointers invalid
	for (size_t i = 1; i < count; i++) {
		
		pde_t *pde = &pd->entries[PAGE_PDE(i)];
		pt_t *pt = (pt_t *)(*pde & PAGE_MASK);
		
		map_pte(pt, PAGE_PTE(i), (void *)(i * PAGE_SIZE));
	}
	
}

//paging_asm.asm
void switch_context(void **old_sp, void *new_sp);


















