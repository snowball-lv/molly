#include <proc.h>
#include <types.h>
#include <pmm.h>
#include <klib.h>
#include <vmm.h>
#include <console.h>
#include <sync.h>

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
	kproc.next		= 0;
	
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
	//omit first page to make 0 pointers invalid
	for (size_t i = 1; i < count; i++) {
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

#define STACK_SIZE	(PAGE_SIZE)

static void process_start() {
	
	//exit critical section
	enable_ints();
	
	current->entry();
	
	kprintfln("process exited");
	
	while(1);
}

static int dummy_entry() {
	kprintfln("in dummy process");
	return 0;
}

static void schedule_process(proc_t *p) {
	proc_t *tmp = current;
	while (tmp->next != 0)
		tmp = tmp->next;
	tmp->next = p;
}

void create_process() {

	//enter critical section
	disable_ints();
	
	int *stack = kmalloc(STACK_SIZE);
	int *top = stack + STACK_SIZE / sizeof(int);
	
	top--;
	*top = (int)process_start;
	
	//init general purpose registers
	for (int i = 0; i < 8; i++) {
		top--;
		*top = 0;
	}
	
	proc_t *p = kmalloc(sizeof(proc_t));
		
	p->id 			= next_pid++;
	p->pd 			= 0; //TODO
	p->heap_base 	= (void *)0x40000000;
	p->heap_top 	= p->heap_base;
	p->sp 			= top;
	p->entry 		= dummy_entry;
	p->next 		= 0;
	
	schedule_process(p);

	//exit critical section
	enable_ints();
}

//proc_asm.asm
void switch_context(void **old_sp, void *new_sp);

void reschedule() {

	//enter cricital section
	disable_ints();

	//kprintfln("reschedule");
	
	proc_t *old_proc = current;
	proc_t *new_proc = old_proc->next;
	
	if (new_proc == 0)
		new_proc = &kproc;
		
	if (new_proc == old_proc) {
		//exit critical section
		enable_ints();
		return;
	}
	
	//kprintfln("switch %d to %d", (int)old_proc->id, (int)new_proc->id);
	
	current = new_proc;
	
	//context switch
	switch_context(&old_proc->sp, new_proc->sp);
	
	//exit critical section
	enable_ints();
}




















