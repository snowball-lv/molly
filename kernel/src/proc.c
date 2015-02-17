#include <proc.h>

#include <paging.h>
#include <pmm.h>
#include <types.h>
#include <kallocator.h>
#include <thread.h>
#include <scheduler.h>
#include <console.h>

static proc_t 	_null_proc;
static thread_t _null_thread;
static pd_t		_null_pd 
	__attribute__((aligned(PAGE_SIZE)));

proc_t *get_null_proc() {
	return &_null_proc;
}

extern none_t _KERNEL_END;

void init_null_proc() {
	
	//init null pd
	memset(&_null_pd, 0, sizeof(pd_t));
	
	//allocate page directory
	_null_proc.pd 	= &_null_pd;
	pd_t *pd 		= &_null_pd;
	
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
	
	//init null thread
	_null_thread.stack 	= 0;
	_null_thread.sp 	= 0;
	_null_thread.entry 	= 0;
	_null_thread.state 	= THREAD_STATE_RUNNING;
	_null_thread.next 	= 0;
	
	//set null thread
	_null_proc.current_thread 	= &_null_thread;
	_null_proc.thread_queue		= 0;
	
	//set null thread
	_null_proc.state = PROC_STATE_RUNNING;
	
	//set next process
	_null_proc.next = 0;
}

static pd_t *create_pd() {

	pd_t *pd = kmalloc_page();
	memset(pd, 0, sizeof(pd_t));
	
	pd_t *cpd = (pd_t *)0xfffff000;
	
	for (size_t i = 0; i < 256; i++)
		pd->entries[i] = cpd->entries[i];
		
	void *pd_phys = virt_to_phys(pd);
	
	map_pde(pd, 1023, pd_phys);

	return pd;
}

void create_process(int (*entry)()) {

	proc_t *p = kmalloc(sizeof(proc_t));
	
	p->pd = create_pd();
	
	thread_t *t = alloc_thread(entry);
	
	t->state = THREAD_STATE_RUNNING;
	
	p->thread_queue 	= t;
	p->current_thread 	= 0;
	
	p->state = PROC_STATE_RUNNING;
	
	p->next = 0;
	
	schedule_process(p);
}

















