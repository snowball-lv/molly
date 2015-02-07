#include <proc.h>
#include <types.h>
#include <pmm.h>
#include <klib.h>

static pid_t next_pid = 1;

static proc_t kproc;
static proc_t *current;

proc_t *get_kproc() { return &kproc; }
proc_t *get_current_proc() { return current; }

extern none_t _HEAP_START;

void init_kproc() {

	current = &kproc;

	kproc.id 		= next_pid++;
	kproc.heap_base = &_HEAP_START;
	kproc.heap_top 	= kproc.heap_base;
	
	//allocate page directory
	kproc.pd = pmm_alloc_block();
	memset(kproc.pd, 0, sizeof(pd_t));
	
	//recursive map last pde
	pde_t *last = &kproc.pd->entries[1023];
	*last |= VMM_PRESENT;
	*last |= VMM_READ_WRITE;
	*last |= (uintptr_t)kproc.pd;
	
	//page count to identity map
	size_t count = (uintptr_t)kproc.heap_base / PAGE_SIZE;
	
	//identity map kernel pages
	for (size_t i = 0; i < count; i++) {
		uintptr_t addr = i * PAGE_SIZE;
		
		pde_t *pde = vmm_get_pde(kproc.pd, addr);
		
		if (*pde == 0) {
			pt_t *pt = pmm_alloc_block();
			memset(pt, 0, sizeof(pt_t));
			*pde |= VMM_PRESENT;
			*pde |= VMM_READ_WRITE;
			*pde |= (uintptr_t)pt;
		}
			
		pt_t *pt = (pt_t *)(*pde & VMM_PAGE_MASK);
		pte_t *pte = vmm_get_pte(pt, addr);
		
		*pte |= VMM_PRESENT;
		*pte |= VMM_READ_WRITE;
		*pte |= (i * PAGE_SIZE);
	}
}






















