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
#include <pe.h>
#include <vfs.h>
#include <debug.h>

extern pd_t boot_pd;

#define MAX_PROCS	(16)

static proc_t procs[MAX_PROCS];
static int cp_num = 0;

proc_t *get_proc(size_t num) {
	return &procs[num];
}

proc_t *cproc() {
	return &procs[cp_num];
}

extern pd_t boot_pd;
void user_main();

//proc_asm.asm
void user_jump(void *ksp);

#define _4MB		(4 * 1024 * 1024)
#define PUSH(v)		(t->ksp--, *t->ksp = (int)(v))
#define UPUSH(v)	(t->usp--, *t->usp = (int)(v))

extern none_t _RAMFS_START;

#define ROUND_UP(v)	((char *)(((uintptr_t)(v) + 7) & ~(7)))

static char *init_start;

static int find_init() {

	char *ptr = (char *)&_RAMFS_START;
	size_t count = *(size_t *)ptr;
	ptr += 4;
	
	for (size_t i = 0; i < count; i++) {
	
		ptr = ROUND_UP(ptr);
		char *name = ptr;
		ptr += strlen(name) + 1;
		
		ptr = ROUND_UP(ptr);
		uint32_t init_size = *(size_t *)ptr;
		ptr += 4;
		
		ptr = ROUND_UP(ptr);
		init_start = ptr;
		ptr += init_size;
		
		if (!strcmp("init.exe", name))
			return 1;
	}
	
	return 0;
}

void run_init() {

	//null proc table
	for (int i = 0; i < MAX_PROCS; i++)
		procs[i].state = S_FREE;

	proc_t *p = &procs[0];
	
	p->state 	= S_USED;
	p->pd	 	= create_pd();
	p->ct_num 	= 0;
	p->cwd		= "/";
		
	load_PDBR(vtp(p->pd));
		
	find_init();
	
	uintptr_t brk = 0;
	entry_f	entry = 0;
	
	load_image(init_start, &entry, &brk);
		
	kprintfln("brk: %x", brk);
	kprintfln("entry: %x", entry);
		
	p->brk = (void *)brk;
	
	for (int i = 0; i < MAX_THREADS; i++)
		p->threads[i].state = S_FREE;
		
	for (int i = 0; i < MAX_THREADS; i++)
		p->files[i].state = S_FREE;
	
	thread_t *t = &p->threads[0];
	
	set_up_thread(t, entry, sbrk(4096));
	
	/*
	
	t->state 	= S_USED;
	t->ustack 	= sbrk(4096);
	t->usp 		= t->ustack + 1024;
	t->kstack	= kmalloc(4096);
	t->ktop 	= t->kstack + 1024;
	t->ksp 		= t->ktop;
	t->entry 	= *entry;
	
	UPUSH(0);	//argv
	UPUSH(0);	//argc
	UPUSH(0);	//ret
	
	PUSH(0x20 | 0x3);	//ss
	PUSH(t->usp);		//esp
	PUSH(read_flags());	//eflags
	PUSH(0x18 | 0x3);	//cs
	PUSH(t->entry);		//eip
	
	*/
	
	set_tss(t->ktop);
	user_jump(t->ksp);
}

//proc_asm.asm
void thread_entry();

void set_up_thread(thread_t *t, entry_f entry, void *stack) {

	t->entry 	= entry;
	t->state 	= S_USED;
	t->ustack 	= stack;
	t->usp 		= t->ustack + 1024;
	t->kstack	= kmalloc(4096);
	t->ktop		= t->kstack + 1024;
	t->ksp 		= t->ktop;
	
	UPUSH(0);	//argv
	UPUSH(0);	//argc
	UPUSH(0);	//ret
	
	PUSH(0x20 | 0x3);	//ss
	PUSH(t->usp);		//esp
	PUSH(read_flags());	//eflags
	PUSH(0x18 | 0x3);	//cs
	PUSH(t->entry);		//eip
	
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

static int fetch_free_proc() {
	
	//find free proc slot
	for (int i = 0; i < MAX_PROCS; i++) {
		
		if (procs[i].state == S_FREE)
			return i;
	}
	
	return -1;
}

static void *psbrk(proc_t *p, size_t size) {
	
	char *cbrk = p->brk;
	char *nbrk = cbrk + size;
	void *ret = cbrk;

	for (;cbrk < nbrk; cbrk += PAGE_SIZE) {
		void *phys = pmm_alloc_block();
		map_page(cbrk, phys, PTE_P | PTE_RW | PTE_U);
	}
	
	p->brk = cbrk;

	return ret;
}

int create_proc(char *wd, char *img) {
	
	//get free proc slot
	int pid = fetch_free_proc();
	
	//no slot found
	if (pid < 0)
		return -1;
		
	//new proc
	proc_t *p = &procs[pid];

	//set wd
	p->cwd = wd;

	//create new proc's pd with kernel pages
	p->pd = create_pd();

	//current proc's pd (indirect due to init)
	pd_t *cpd = (pd_t *)0xfffff000;
	void *cpd_phys = vtp(cpd);
	
	//switch to new pd
	load_PDBR(vtp(p->pd));
	
	//load bianry image into lower memory
	uintptr_t brk = 0;
	entry_f entry = 0;
	int err = load_image(img, &entry, &brk);
	
	//failed to load image
	if (err < 0) {
		//switch back to callers pd
		load_PDBR(cpd_phys);
		return -1;
	}
		
	p->brk 		= (void *)brk;
	p->ct_num 	= 0;

	logfln("brk: %x", p->brk);

	//clear thread table
	for (int i = 0; i < MAX_THREADS; i++)
		p->threads[i].state = S_FREE;

	//clear file table
	for (int i = 0; i < MAX_THREADS; i++)
		p->files[i].state = S_FREE;
		
	//set up initial thread
	thread_t *t = &p->threads[0];
	set_up_thread(t, entry, psbrk(p, 4096));

	//switch back to callers pd
	load_PDBR(cpd_phys);

	//make process ready for sched.
	p->state = S_USED;
	
	return pid;
}

static int runnable_procs() {

	int sum = 0;

	for (int i = 0; i < MAX_PROCS; i++) {

		proc_t *p = get_proc(i);

		if (p->state == S_USED)
			sum++;
	}

	return sum;
}

void reschedule() {

	if (runnable_procs() <= 1)
		return;

	//logfln("resched");

	int start = (cp_num + 1) % MAX_PROCS;
	int next = cp_num;

	int i = start;
	while (1) {

		proc_t *p = &procs[i];

		if (p->state == S_USED) {
			next = i;
			break;
		}

		i = (i + 1) & MAX_PROCS;
	}

	if (cp_num == next)
		return;

	//logfln("switch: %d -> %d", cp_num, next);

	proc_t *cp = get_proc(cp_num);
	thread_t *ct = &cp->threads[cp->ct_num];

	proc_t *np = get_proc(next);
	thread_t *nt = &np->threads[np->ct_num];

	load_PDBR(vtp(np->pd));

	cp_num = next;

	set_tss(nt->ktop);
	switch_context((void **)&ct->ksp, nt->ksp);
}	




















