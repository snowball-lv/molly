#include <proc.h>

#include <paging.h>
#include <pmm.h>
#include <types.h>
#include <kalloc.h>
#include <console.h>
#include <molly.h>
#include <param.h>
#include <gdt.h>
#include <idt.h>

#define PDE_SHIFT 		(22)

__attribute__((aligned(PAGE_SIZE)))
pd_t init_pd = {
	.entries = {
		[0]
			= (0) | PTE_P | PTE_RW | PTE_PS | PTE_U,
		[KERNEL_BASE >> PDE_SHIFT]
			= (0) | PTE_P | PTE_RW | PTE_PS
	}
};

#define MAX_PROCS	(16)

static proc_t procs[MAX_PROCS];
static int cp_num = 0;

proc_t *get_proc(size_t num) {
	return &procs[num];
}

proc_t *cproc() {
	return &procs[cp_num];
}

extern pd_t init_pd;
void user_main();

//proc_asm.asm
void user_jump(void *ksp);

#define _4MB		(4 * 1024 * 1024)
#define PUSH(v)		(t->ksp--, *t->ksp = (int)(v))

void user_mode() {

	for (int i = 0; i < MAX_PROCS; i++)
		procs[i].state = S_FREE;

	proc_t *p = &procs[0];
	
	p->state	= S_USED;
	p->pd 		= &init_pd;
	p->brk 		= (void *)_4MB;
	p->ct_num 	= 0;

	for (int i = 0; i < MAX_THREADS; i++)
		p->threads[i].state = S_FREE;
	
	thread_t *t = &p->threads[0];
	t->state 	= S_USED;
	t->ustack 	= sbrk(4096);
	t->usp 		= t->ustack + 1024;
	t->kstack	= kmalloc(4096);
	t->ktop 	= t->kstack + 1024;
	t->ksp 		= t->ktop;
	t->entry 	= 0;
	
	PUSH(0x20 | 0x3);				//ss
	PUSH(t->usp);					//esp
	PUSH(read_flags());				//eflags
	PUSH(0x18 | 0x3);				//cs
	PUSH(user_main - KERNEL_OFF);	//eip
	
	set_tss(t->ktop);
	
	user_jump(t->ksp);
}

//proc_asm.asm
void thread_entry();

void set_up_thread(thread_t *t, int (*entry)()) {

	t->entry 	= entry;
	t->state 	= S_USED;
	t->ustack 	= sbrk(4096);
	t->usp 		= t->ustack + 1024;
	t->kstack	= kmalloc(4096);
	t->ktop		= t->kstack + 1024;
	t->ksp 		= t->ktop;
	
	PUSH(0x20 | 0x3);				//ss
	PUSH(t->usp);					//esp
	PUSH(read_flags());				//eflags
	PUSH(0x18 | 0x3);				//cs
	PUSH(t->entry - KERNEL_OFF);	//eip
	
	PUSH(thread_entry);
	PUSH(read_flags());
	for (int i = 0; i < 8; i++)
		PUSH(0);
}

pd_t *clone_pd();
void load_PDBR(pd_t *pd);
void *vtp(void *virt);
void fork_child_ret();

int proc_clone(trapframe_t *tf) {

	proc_t *np = &procs[1];
	proc_t *cp = cproc();
	
	np->state 	= S_USED;
	np->pd 		= clone_pd();
	np->brk 	= cp->brk;
	np->ct_num	= 0;
	
	for (int i = 0; i < MAX_THREADS; i++)
		np->threads[i].state = S_FREE;
		
	thread_t *t = &np->threads[np->ct_num];
	thread_t *ct = &cp->threads[cp->ct_num];
	
	t->state 	= S_USED;
	t->ustack 	= ct->ustack;
	t->usp 		= ct->usp;
	t->kstack 	= kmalloc(4096);
	t->ktop		= t->kstack + 1024;
	t->ksp		= t->ktop;
	t->entry	= 0;
	
	PUSH(0x20 | 0x3);		//ss
	PUSH(tf->uesp);			//esp
	PUSH(tf->eflags);		//eflags
	PUSH(0x18 | 0x3);		//cs
	PUSH(tf->eip);			//eip
	
	PUSH(fork_child_ret);
	PUSH(read_flags());
	for (int i = 0; i < 8; i++)
		PUSH(0);
	
	return 1;
}


















