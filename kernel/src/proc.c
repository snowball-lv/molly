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

int create_proc(char *path, char **args);

void run_init() {

	//null proc table
	for (int i = 0; i < MAX_PROCS; i++)
		procs[i].state = S_FREE;

	proc_t *p = &procs[0];
	
	p->state 	= S_USED;
	p->pd	 	= create_pd();
	p->ct_num 	= 0;
		
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
	
	set_up_thread(t, entry);
	
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

void set_up_thread(thread_t *t, entry_f entry) {

	t->entry 	= entry;
	t->state 	= S_USED;
	t->ustack 	= sbrk(4096);
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

	//free pid
	int pid = -1;
	
	//find free proc slot
	for (int i = 0; i < MAX_PROCS; i++) {
		if (procs[i].state == S_FREE) {
			//reserve slot
			procs[i].state = S_USED;
			pid = i;
			break;
		}
	}
	
	return pid;
}

int create_proc(char *path, char **args) {
	
	//get free proc slot
	int pid = fetch_free_proc();
	
	//no slot found
	if (pid < 0)
		return -1;
		
	//new proc
	proc_t *p = &procs[pid];
	
	//create new proc's pd with kernel pages
	p->pd = create_pd();
	
	//current proc's pd (indirect due to init)
	pd_t *cpd = (pd_t *)0xfffff000;
	
	//switch to new pd
	load_PDBR(vtp(p->pd));
	
	//load bianry image into lower memory
	uintptr_t brk = 0;
	entry_f entry = 0;
	int err = load_image(init_start, &entry, &brk);
	
	//failed to load image
	if (err < 0) {
		//switch back to callers pd
		load_PDBR(vtp(cpd));
		return -1;
	}
		
	p->brk 		= (void *)brk;
	p->ct_num 	= 0;

	//clear thread table
	for (int i = 0; i < MAX_THREADS; i++)
		p->threads[i].state = S_FREE;
		
	//set up initial thread
	thread_t *t = &p->threads[0];
	set_up_thread(t, entry);
	
	//switch back to callers pd
	load_PDBR(vtp(cpd));
	
	return pid;
}






















